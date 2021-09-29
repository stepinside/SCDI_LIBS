#include <stdio.h>
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <tb.h>
#include <uart.h>
#include <eeprom.h>
#include "dbMc.h"

// Pins to control the DiscBot's H-bridge
#define MOTOR_LEFT_ENA   (1 << 3)       // PinL.3
#define MOTOR_LEFT_IN1A  (1 << 5)       // PinL.5
#define MOTOR_LEFT_IN2A  (1 << 1)       // PinL.1
#define MOTOR_RIGHT_ENB  (1 << 4)       // PinL.4
#define MOTOR_RIGHT_IN1B (1 << 0)       // PinL.0
#define MOTOR_RIGHT_IN2B (1 << 2)       // PinL.2

#define MAX_OCR                     40000   // the maximum OCR value
#define MIN_OCR                     4000    // the minimal OCR value, below the DiscBot does not move at all; the value was obtained by experiments

#define OCR_LEFT                    OCR5A
#define OCR_RIGHT                   OCR5B

#define EPROM_ADDRESS               0x00    // the EPROM address where the configuration values of the motion control library are stored

#define OCR_MAX_STEPSIZE            30      // straight -> the OCR step size, when the speed and direction of the DiscBot are set
#define OCR_MIN_STEPSIZE            15
#define OCR_MANEUVER_STEPSIZE       100     // the OCR step size, when the DiscBot perform a maneuver when performing maneuvers, the regulation must be faster

#define MAX_BRAKE_DURATION          3       // the braking duration is a multiple of SPEED_UPDATE_RATE_MS

#define SPEED_UPDATE_RATE_MS        50

#define SPEED_CORRECTION_FACTOR     16       // this factor determines how fast the DiscBot's speed shall be adapted to the desired value; the factor was obtained by experiments.
#define PULSES_PER_ROTATION         234     // the number of pulses a wheel sends per rotation
#define WHEEL_CIRCUMFERENCE_MM      217     // the standard circumference of the DiscBot's wheels

static volatile int16_t  _dbMc_ticksLeft = 0;   // counter of the ticks of the left wheel's encoder
static volatile int16_t  _dbMc_ticksRight = 0;  // counter of the ticks of the right wheel's encoder
static volatile uint16_t _dbMc_targetTicksLeft; // the number of ticks the left wheel must send withing the update rate to reach the wheel's target speed
static volatile uint16_t _dbMc_targetTicksRight;// the number of ticks the right wheel must send withing the update rate to reach the wheel's target speed

static volatile int8_t  _dbMc_direction  = 0;   // the DiscBot's target direction
static volatile int16_t _dbMc_speed_cmps = 0;   // the DiscBot's target speed in centimeters per second
static volatile int16_t _dbMc_speedLeft_cmps;   // the target speed of the left wheel
static volatile int16_t _dbMc_speedRight_cmps;  // the target speed of the right wheel
static volatile uint16_t _dbMc_wheelCircumference_mm = WHEEL_CIRCUMFERENCE_MM;  // the wheels' circumference in mm

static volatile uint8_t _dbMc_initialized = 0;  // has the library already been initialized by calling dbMc_init

static volatile uint16_t _dbMc_ocrStepsize  = OCR_MIN_STEPSIZE;    // the step size by which the DiscBot's OCR values shall be adapted

// callback to be called, when the DiscBot's speed is to be changed
static uint8_t (*_dbMc_speedChangedCallback)(int16_t oldSpeed, int16_t newSpeed) = NULL;
// callback to be called, when the DiscBot's direction is to be changed
static uint8_t (*_dbMc_directionChangedCallback)(int8_t oldDirection, int8_t newDirection) = NULL;

// when performing maneuvers, the number of ticks per encoder are calculated and the movement is stopped,
// when the necessary number of ticks occurred
static volatile uint32_t _dbMc_maxTicksLeft = 0;
static volatile uint32_t _dbMc_maxTicksRight = 0;

static void (*_dbMc_brakeLeftCallback)() = NULL;        // pointer to the function to be called when the left wheel stopped turning
static void (*_dbMc_brakeRightCallback)() = NULL;       // pointer to the function to be called when the right wheel stopped turning
static void (*_dbMc_brakeCallback)() = NULL;            // pointer to the function to be called when both wheels stopped turning
static volatile uint8_t _dbMc_brakePhase;               // counter of the wheels that stopped turning
static volatile uint8_t _dbMc_brakeStepLeft=0;          // counts how long the left wheel is already braking
static volatile uint8_t _dbMc_brakeStepRight=0;         // counts how long the right wheel is already braking

// ----------------------------------------------------------------------------
// function prototypes
// ----------------------------------------------------------------------------
uint16_t dbMc_calcAndUpdateSpeed();
// ----------------------------------------------------------------------------

void dbMc_init()
{
  if (_dbMc_initialized)    return;
  _dbMc_initialized = 1;

  // read the wheels' circumference from the EPROM
  _dbMc_wheelCircumference_mm = (uint16_t)eeprom_read(EPROM_ADDRESS) | (uint16_t)eeprom_read(EPROM_ADDRESS+1);
  if ((_dbMc_wheelCircumference_mm < 180) || (_dbMc_wheelCircumference_mm >= 260))
  {
    _dbMc_wheelCircumference_mm = WHEEL_CIRCUMFERENCE_MM;
  }

  // the pins to control the h-bridge must be outputs
  DDRL |= (MOTOR_LEFT_ENA | MOTOR_LEFT_IN1A | MOTOR_LEFT_IN2A | MOTOR_RIGHT_ENB | MOTOR_RIGHT_IN1B | MOTOR_RIGHT_IN2B);

  TCCR5A = (1 << WGM51);                                // fast PWM mode
  TCCR5B = ((1 << WGM53) | (1 << WGM52));

  TIMSK5 = 0;                                           // disable all interrupts
  OCR_LEFT = OCR_RIGHT = 0;                             // stop the motors

  // 62.5ns * 320.000 = 20ms
  // 62.5ns * 8(PS) * 40000(ICR) = 20ms
  ICR5 = MAX_OCR-1;

  TCCR5A |= ((1 << COM5A1) | (1 << COM5B1));            // provide hardware composed PWM signals at OC5A and OC5B
  TCCR5B |= (1 << CS51);                                // PS=8

  EICRB |= ((1 << ISC41) | (1 << ISC51));               // enable the encoders' interrupts
  EICRB &= ~((1 << ISC40) | (1 << ISC50));
  EIMSK |= ((1 << INT4) | (1 << INT5));

  tb_register(&dbMc_calcAndUpdateSpeed, SPEED_UPDATE_RATE_MS); // the speed shall be calculated and updated regularly

  sei();
}

uint8_t dbMc_isInitialized()
{
  return _dbMc_initialized;
}

uint16_t _dbMc_calcOcrStepsize(int16_t speed_cmps)
{
  if (speed_cmps < 0)   speed_cmps = -speed_cmps;

  return (OCR_MAX_STEPSIZE-OCR_MIN_STEPSIZE)*speed_cmps / 200 + OCR_MIN_STEPSIZE;
}
// ----------------------------------------------------------------------------
// speed and direction functions
// ----------------------------------------------------------------------------
void dbMc_setSpeedLeft(int16_t speed_cmps)
{
  if (_dbMc_speedLeft_cmps == speed_cmps)
  {
    return;
  }

  _dbMc_maxTicksLeft = 0;                               // when the left wheel's speed is set, no maneuver is taking place
  _dbMc_brakeStepLeft = 0;                              // there's no need to brake

  if (speed_cmps < -200)    speed_cmps = -200;
  if (speed_cmps > 200)     speed_cmps = 200;

  // if direction changed ... added 25.3.2020
  if ((_dbMc_speedLeft_cmps>0 && speed_cmps<0) || (_dbMc_speedLeft_cmps<0 && speed_cmps>0))
  {
    OCR_LEFT = MIN_OCR;
    _dbMc_ticksLeft = 0;
  }

  _dbMc_speedLeft_cmps = speed_cmps;
  _dbMc_ocrStepsize = _dbMc_calcOcrStepsize(speed_cmps);

  if (speed_cmps > 0)                                   // drive forwards
  {
    PORTL &= ~MOTOR_LEFT_IN1A;
    PORTL |=  MOTOR_LEFT_IN2A;

    // calculate the ticks that need to take place within 100ms to reach the target speed.
    _dbMc_targetTicksLeft = (int32_t)speed_cmps * PULSES_PER_ROTATION * SPEED_UPDATE_RATE_MS / _dbMc_wheelCircumference_mm / 100;
  }
  else if (speed_cmps < 0)                              // drive backwards
  {
    PORTL |= MOTOR_LEFT_IN1A;
    PORTL &= ~MOTOR_LEFT_IN2A;
    _dbMc_targetTicksLeft = (int32_t)-speed_cmps * PULSES_PER_ROTATION * SPEED_UPDATE_RATE_MS / _dbMc_wheelCircumference_mm / 100;
  }
  else                                                  // brake the wheel
  {
    _dbMc_targetTicksLeft = 0;
  }
}
void dbMc_setSpeedRight(int16_t speed_cmps)
{
  if (_dbMc_speedRight_cmps == speed_cmps)
  {
    return;
  }

  _dbMc_maxTicksRight = 0;
  _dbMc_brakeStepRight = 0;

  if (speed_cmps < -200)    speed_cmps = -200;
  if (speed_cmps > 200)     speed_cmps = 200;

  // if direction changed ... added 25.3.2020
  if ((_dbMc_speedRight_cmps>0 && speed_cmps<0) || (_dbMc_speedRight_cmps<0 && speed_cmps>0))
  {
    OCR_RIGHT = MIN_OCR;
    _dbMc_ticksRight = 0;
  }

  _dbMc_speedRight_cmps = speed_cmps;
  _dbMc_ocrStepsize = _dbMc_calcOcrStepsize(speed_cmps);

  if (speed_cmps > 0)
  {
    PORTL &= ~MOTOR_RIGHT_IN1B;
    PORTL |=  MOTOR_RIGHT_IN2B;
    _dbMc_targetTicksRight = (int32_t)speed_cmps * PULSES_PER_ROTATION * SPEED_UPDATE_RATE_MS / _dbMc_wheelCircumference_mm / 100;
  }
  else if (speed_cmps < 0)
  {
    PORTL |= MOTOR_RIGHT_IN1B;
    PORTL &= ~MOTOR_RIGHT_IN2B;
    _dbMc_targetTicksRight = (int32_t)-speed_cmps * PULSES_PER_ROTATION * SPEED_UPDATE_RATE_MS / _dbMc_wheelCircumference_mm / 100;
  }
  else
  {
    _dbMc_targetTicksRight = 0;
  }
}
void dbMc_setSpeedAndDirection(int16_t speed_cmps, int8_t direction)
{
  if (speed_cmps > 200)    speed_cmps = 200;            // check the new speed value
  if (speed_cmps < -200)   speed_cmps = -200;

  // in case a callback was registered, inform about the forthcoming change of the speed
  if (!_dbMc_speedChangedCallback || _dbMc_speedChangedCallback(_dbMc_speed_cmps, speed_cmps))
  {
    _dbMc_speed_cmps = speed_cmps;
  }

  if (direction > 100)    direction = 100;              // check the new direction value
  if (direction < -100)   direction = -100;

  // in case a callback was registered, inform about the forthcoming change of the direction
  if (!_dbMc_directionChangedCallback || _dbMc_directionChangedCallback(_dbMc_direction, direction))
  {
    _dbMc_direction = direction;
  }

  // calculate the speed of the left and right wheel depending on the direction
  if (_dbMc_direction > 0)
  {
    dbMc_setSpeedLeft((int16_t)_dbMc_speed_cmps * (50-_dbMc_direction) / 50);
    dbMc_setSpeedRight(_dbMc_speed_cmps);
  }
  else if (_dbMc_direction < 0)
  {
    dbMc_setSpeedLeft(_dbMc_speed_cmps);
    dbMc_setSpeedRight((int16_t)_dbMc_speed_cmps * (50+_dbMc_direction) / 50);
  }
  else
  {
    dbMc_setSpeedLeft(_dbMc_speed_cmps);
    dbMc_setSpeedRight(_dbMc_speed_cmps);
  }
}
void dbMc_setSpeed(int16_t speed_cmps)
{
  dbMc_setSpeedAndDirection(speed_cmps, _dbMc_direction);
}
void dbMc_setDirection(int8_t direction)
{
  dbMc_setSpeedAndDirection(_dbMc_speed_cmps, direction);
}

int16_t dbMc_getSpeed()
{
  return _dbMc_speed_cmps;
}
int16_t dbMc_getSpeedLeft()
{
  return _dbMc_speedLeft_cmps;
}
int16_t dbMc_getSpeedRight()
{
  return _dbMc_speedRight_cmps;
}
int8_t dbMc_getDirection()
{
  return _dbMc_direction;
}

void dbMc_registerSpeedChangedCallback(uint8_t (*changedCallback)(int16_t oldSpeed_cmps, int16_t newSpeed_cmps))
{
  _dbMc_speedChangedCallback = changedCallback;
}
void dbMc_registerDirectionChangedCallback(uint8_t (*changedCallback)(int8_t oldDirection, int8_t newDirection))
{
  _dbMc_directionChangedCallback = changedCallback;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// brake functions
// ----------------------------------------------------------------------------
void dbMc_brakeLeft(void (*doneCallback)())
{
  _dbMc_brakeLeftCallback = doneCallback;
  dbMc_setSpeedLeft(0);
}
void dbMc_brakeRight(void (*doneCallback)())
{
  _dbMc_brakeRightCallback = doneCallback;
  dbMc_setSpeedRight(0);
}
void _dbMc_brakeReady()
{
  void (*brakeCallback)();

  if (++_dbMc_brakePhase == 2)
  {
    if (_dbMc_brakeCallback)
    {
      brakeCallback = _dbMc_brakeCallback;
      _dbMc_brakeCallback = NULL;
      (*brakeCallback)();
    }
  }
}
void dbMc_brake(void (*doneCallback)())
{
  _dbMc_brakePhase = 0;
  _dbMc_brakeCallback = doneCallback;
  dbMc_brakeLeft(_dbMc_brakeReady);
  dbMc_brakeRight(_dbMc_brakeReady);
}

// ----------------------------------------------------------------------------
// maneuvers
// ----------------------------------------------------------------------------
void dbMc_move(uint16_t distance_mm, int16_t speed_cmps, void (*doneCallback)())
{
  dbMc_setSpeedAndDirection(speed_cmps, 0);

  // calculate the number of ticks necessary for the desired distance
  _dbMc_maxTicksLeft = _dbMc_maxTicksRight = (uint32_t)distance_mm * PULSES_PER_ROTATION / _dbMc_wheelCircumference_mm;

  // when performing maneuvers, the OCR values need to be adapted faster
  _dbMc_ocrStepsize = OCR_MANEUVER_STEPSIZE;

  // since this maneuver will be finished by braking the wheels, the brake callback can used
  _dbMc_brakePhase = 0;
  _dbMc_brakeCallback = doneCallback;
  _dbMc_brakeLeftCallback = _dbMc_brakeReady;
  _dbMc_brakeRightCallback = _dbMc_brakeReady;
}
void dbMc_rotate(int16_t angle, uint8_t speed_cmps, void (*doneCallback)())
{
  // set the wheels' speed, depending on the rotation angle
  if (angle > 0)
  {
    dbMc_setSpeedLeft(speed_cmps);
    dbMc_setSpeedRight(-speed_cmps);
  }
  else
  {
    dbMc_setSpeedLeft(-speed_cmps);
    dbMc_setSpeedRight(speed_cmps);
    angle = -angle;
  }

  // 360° =^= 2*r*PI (r=approx. 103cm) =^= 650mm; 234 Ticks/Umdrehung; 234 Ticks =^= dbMc_circumference_mm;
  // 360° =^= 650mm =^= 234*650mm/dbMc_circumference_mm Ticks
  // angle° = ?
  // angle = 234*650mm/360/dbMc_circumference_mm Ticks
  // ticks = angle * 234 * 691mm / dbMc_circumference_mm / 360
  _dbMc_maxTicksLeft = _dbMc_maxTicksRight = (int32_t)angle * PULSES_PER_ROTATION*650 / 360 / _dbMc_wheelCircumference_mm;

  // when performing maneuvers, the OCR values need to be adapted faster
  _dbMc_ocrStepsize = OCR_MANEUVER_STEPSIZE;

  // since this maneuver will be finished by braking the wheels, the brake callback can used
  _dbMc_brakePhase = 0;
  _dbMc_brakeCallback = doneCallback;
  _dbMc_brakeRightCallback = &_dbMc_brakeReady;
  _dbMc_brakeLeftCallback = &_dbMc_brakeReady;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// speed regulation functions
// ----------------------------------------------------------------------------
uint16_t dbMc_calcAndUpdateSpeed()
{
  int16_t diffCntLeft, diffCntRight;
  int16_t stepsizeLeft, stepsizeRight;
  int32_t newOCR;
  void (*brakeCallback)();

  /*
  int16_t speedLeft_cms = (int32_t)_dbMc_ticksLeft * 100 * _dbMc_wheelCircumference_mm / PULSES_PER_ROTATION / SPEED_UPDATE_RATE_MS;
  int16_t speedRight_cms = (int32_t)_dbMc_ticksRight * 100 * _dbMc_wheelCircumference_mm / PULSES_PER_ROTATION / SPEED_UPDATE_RATE_MS;
  */

  // calculate the difference between the desired ticks and the actual ticks
  diffCntLeft = _dbMc_targetTicksLeft - _dbMc_ticksLeft;
  diffCntRight = _dbMc_targetTicksRight - _dbMc_ticksRight;

  // reset the tick counters
  _dbMc_ticksLeft = _dbMc_ticksRight = 0;

  // calculate values by which the OCR values shall be corrected
  stepsizeLeft = diffCntLeft * SPEED_CORRECTION_FACTOR;
  stepsizeRight = diffCntRight * SPEED_CORRECTION_FACTOR;

  if (_dbMc_targetTicksLeft)                            // only if the left wheel shall turn
  {
    newOCR = (int32_t)OCR_LEFT + stepsizeLeft;
    if (newOCR > MAX_OCR)   newOCR = MAX_OCR;
    if (newOCR < MIN_OCR)   newOCR = MIN_OCR;
    OCR_LEFT = newOCR;
  }
  else
  {
    if (OCR_LEFT != 0)                                  // brake the left wheel
    {
      if (_dbMc_brakeStepLeft++ == 0)                   // when starting to brake, change the rotation direction of the left wheel
      {
        PORTL ^= MOTOR_LEFT_IN1A;
        PORTL ^= MOTOR_LEFT_IN2A;
      }
      if (_dbMc_brakeStepLeft == MAX_BRAKE_DURATION)    // stop braking after a certain time
      {
        OCR_LEFT = 0;
        PORTL |= MOTOR_LEFT_IN1A;
        PORTL |= MOTOR_LEFT_IN2A;
      }
    }
    else if (_dbMc_brakeStepLeft)                       // callback, when the left wheel stopped turning
    {
      _dbMc_brakeStepLeft = 0;
      if(_dbMc_brakeLeftCallback)
      {
        brakeCallback = _dbMc_brakeLeftCallback;
        _dbMc_brakeLeftCallback = NULL;
        brakeCallback();
      }
    }
  }

  if (_dbMc_targetTicksRight)                           // the same applies to the right wheel
  {
    newOCR = (int32_t)OCR_RIGHT + stepsizeRight;
    if (newOCR > MAX_OCR)   newOCR = MAX_OCR;
    if (newOCR < MIN_OCR)   newOCR = MIN_OCR;
    OCR_RIGHT = newOCR;
  }
  else
  {
    if (OCR_RIGHT != 0)
    {
      if (_dbMc_brakeStepRight++ == 0)
      {
        PORTL ^= MOTOR_RIGHT_IN1B;
        PORTL ^= MOTOR_RIGHT_IN2B;
      }
      if (_dbMc_brakeStepRight == MAX_BRAKE_DURATION)
      {
        OCR_RIGHT = 0;
        PORTL |= MOTOR_RIGHT_IN1B;
        PORTL |= MOTOR_RIGHT_IN2B;
      }
    }
    else if (_dbMc_brakeStepRight)
    {
      _dbMc_brakeStepRight = 0;
      if(_dbMc_brakeRightCallback)
      {
        brakeCallback = _dbMc_brakeRightCallback;
        _dbMc_brakeRightCallback = NULL;
        brakeCallback();
      }
    }
  }

// static uint8_t temp[64];
//  sprintf(temp, "{%x-%x}", _dbMc_brakeLeftCallback, _dbMc_brakeRightCallback);
//  sprintf(temp, "left: %4d->%4d, right: %4d->%4d\n", _dbMc_speedLeft_cmps, speedLeft_cms, _dbMc_speedRight_cmps, speedRight_cms);
//  uart0_msg(temp);


  return SPEED_UPDATE_RATE_MS;                          // recall this function regularly
}

ISR(INT5_vect)                                          // left encoder
{
  int32_t newOCR;

  _dbMc_ticksLeft++;
  if (_dbMc_maxTicksLeft)                               // when performing a maneuver, check if the necessary tick count was reached
  {
    _dbMc_maxTicksLeft--;
    if (!_dbMc_maxTicksLeft)
    {
      dbMc_brakeLeft(_dbMc_brakeLeftCallback);          // if yes, then brake the left wheel
    }
  }

  // correct the left wheel's OCR value
  // this regulation is only done, when a maneuver is performed or when the DiscBot shall move straight forward/backward
  if (_dbMc_speedLeft_cmps && (_dbMc_maxTicksLeft || (_dbMc_targetTicksLeft == _dbMc_targetTicksRight)))
  {
    newOCR = (int32_t)OCR_LEFT - _dbMc_ocrStepsize;
    if (newOCR < MIN_OCR)     newOCR = MIN_OCR;
    OCR_LEFT = newOCR;

    if (_dbMc_speedRight_cmps)
    {
      newOCR = (int32_t)OCR_RIGHT + _dbMc_ocrStepsize;
      if (newOCR > MAX_OCR)   newOCR = MAX_OCR;
      OCR_RIGHT = newOCR;
    }
  }
}

ISR(INT4_vect)                                          // right encoder
{
  int32_t newOCR;

  _dbMc_ticksRight++;
  if (_dbMc_maxTicksRight)
  {
    _dbMc_maxTicksRight--;
    if (!_dbMc_maxTicksRight)
    {
      dbMc_brakeRight(_dbMc_brakeRightCallback);
    }
  }

  if (_dbMc_speedRight_cmps && (_dbMc_maxTicksRight || (_dbMc_targetTicksLeft == _dbMc_targetTicksRight)))
  {
    newOCR = (int32_t)OCR_RIGHT - _dbMc_ocrStepsize;
    if (newOCR < MIN_OCR)     newOCR = MIN_OCR;
    OCR_RIGHT = newOCR;

    if (_dbMc_speedLeft_cmps)
    {
      newOCR = (int32_t)OCR_LEFT + _dbMc_ocrStepsize;
      if (newOCR > MAX_OCR)   newOCR = MAX_OCR;
      OCR_LEFT = newOCR;
    }
  }
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// misc functions
// ----------------------------------------------------------------------------
uint8_t dbMc_setWheelCircumference(uint8_t circumference_mm)
{
  if ((circumference_mm < 180) || (circumference_mm >= 230))  return 0;

  _dbMc_wheelCircumference_mm = circumference_mm;
  eeprom_write(EPROM_ADDRESS, (uint8_t)circumference_mm);
  eeprom_write(EPROM_ADDRESS+1, (uint8_t)(circumference_mm >> 8));
  return 1;
}
// ----------------------------------------------------------------------------
