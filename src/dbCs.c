#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <eeprom.h>
#include <tb.h>
#include <uart.h>

#include <dbLs.h>
#include <dbLed.h>
#include <dbBtn.h>

#include "dbCs.h"

enum DbCsColorChannel
{
    DB_CS_COLORCHANNEL_RED = 0,
    DB_CS_COLORCHANNEL_BLUE = 1,
    DB_CS_COLORCHANNEL_CLEAR = 2,
    DB_CS_COLORCHANNEL_GREEN = 3
};

enum DbCsSensor
{
    DB_CS_SENSOR_LEFT = 0,
    DB_CS_SENSOR_MIDDLE = 1,
    DB_CS_SENSOR_RIGHT = 2
};

enum DbCsFrequency
{
    DB_CS_FREQUENCY_OFF = 0,
    DB_CS_FREQUENCY_12K = 1,
    DB_CS_FREQUENCY_120K = 2,
    DB_CS_FREQUENCY_600K = 3
};

enum DbCsMode
{
    DB_CS_CALIBRATE,
    DB_CS_REGISTER,
    DB_CS_MEASURE
};

#define DB_CS_OCR_STEP 10
#define DB_CS_OCR_MAX 1000

volatile uint8_t _dbCs_values[DB_CS_DATA_SIZE];
volatile uint8_t _dbCs_colorRefs[DB_CS_MAX_COLORS][DB_CS_DATA_SIZE];
volatile uint16_t _dbCs_ocrValues[DB_CS_DATA_SIZE];
volatile uint8_t _dbCs_colorChannel = 0;
volatile uint8_t _dbCs_colorNo = 0;
volatile uint8_t _dbCs_sensor = 0;
volatile uint8_t _dbCs_initialized = 0;

volatile uint8_t _dbCs_registrationFinished;

void (*_dbCs_calibratedCallback)(uint16_t calibrationData[DB_CS_DATA_SIZE]);
void (*_dbCs_getColorsCallback)(const struct DbCsColors *colors);
void (*_dbCs_getColorIndexesCallback)(const struct DbCsColors *colors, const struct DbCsColorIndexes *colorIndexes, const struct DbCsColorIndexesQuality *colorIndexesQuality);
void (*_dbCs_colorIndexesChangedCallback)(const struct DbCsColors *colors, const struct DbCsColorIndexes *colorIndexes, const struct DbCsColorIndexesQuality *colorIndexesQuality);
void (*_dbCs_registeredCallback)(int8_t colorIndex, const struct DbCsColors *colors);

volatile enum DbCsMode _dbCs_mode = DB_CS_CALIBRATE;
volatile uint16_t _dbCs_overflow = 0;
volatile uint16_t _dbCs_retriggerTime_ms;
volatile uint8_t _dbCs_measureHandle = 0;
volatile uint8_t _dbCs_minColorIndexesQuality = 5;

volatile struct DbCsColorIndexes _dbCs_colorIndexes;
volatile struct DbCsColorIndexes _dbCs_lastColorIndexes = {-1, -1, -1};
volatile struct DbCsColorIndexesQuality _dbCs_colorIndexesQuality;

void _dbCs_whiteLeds(uint8_t onOff)
{
    PORTA &= ~(1 << 6);
    PORTA |= (onOff << 6);
}

void dbCs_setLeds(uint8_t onOff)
{
    _dbCs_whiteLeds(onOff);
}

void _dbCs_setSensor(enum DbCsSensor sensor)
{
    PORTA &= ~((1 << 4) | (1 << 5));
    PORTA |= (sensor << 4);
}
void _dbCs_setColor(enum DbCsColorChannel color)
{
    PORTA &= ~((1 << 2) | (1 << 3));
    PORTA |= (color << 2);
};
void _dbCs_setFrequency(enum DbCsFrequency frequency)
{
    PORTA &= ~((1 << 0) | (1 << 1));
    PORTA |= (frequency << 0);
};

void _dbCs_bluePressed(uint8_t state)
{
    if (state)
    {
        if (!dbCs_registerColor(NULL))
        {
            _dbCs_registrationFinished = 1;
        }
        dbLs_playSong(dbLs_beep, DB_LS_REGULAR, NULL);
    }
}
void _dbCs_redPressed(uint8_t state)
{
    if (state)
    {
        _dbCs_registrationFinished = 1;
    }
}

void dbCs_init()
{
    uint8_t i, j;
    char text[32];

    if (_dbCs_initialized)
        return;
    _dbCs_initialized = 1;

    dbLs_init();
    dbLed_init();
    dbBtn_init();

    // --------------------------------------------------------------------------
    DDRA = (1 << 6);
    DDRA |= ((1 << 4) | (1 << 5));
    DDRA |= ((1 << 2) | (1 << 3));
    DDRA |= ((1 << 0) | (1 << 1));
    // --------------------------------------------------------------------------

    // --------------------------------------------------------------------------
    // configure the color sensor
    // --------------------------------------------------------------------------
    _dbCs_whiteLeds(0);
    _dbCs_setSensor(DB_CS_SENSOR_MIDDLE);
    _dbCs_setColor(DB_CS_COLORCHANNEL_CLEAR);
    _dbCs_setFrequency(DB_CS_FREQUENCY_600K);

    // --------------------------------------------------------------------------
    // configure timer/counter0
    // --------------------------------------------------------------------------
    TCCR0A = (1 < WGM01); // CTC mode + turn timer/counter0 off
    TCCR0B = 0x00;
    TIMSK0 = (1 << OCIE0A); // enable the compare match interrupt
    OCR0A = 16 - 1;
    TCNT0 = 0; // reset timer/counter0

    // --------------------------------------------------------------------------
    // configure timer3
    // --------------------------------------------------------------------------
    // the frequency to start the white balance is 1/10 of the assumed max frequency of
    // white; this assures that there will be overflows when starting the white balancing
    // 1/10 of 300kHz = 30kHz which should correspond to 15 (max. value)
    // 1/30kHz * 15 = 500us
    // 62.5ns * x = 500us
    // 62.5ns * 8000 = 500us
    // 62.5ns * 8(PS) * 1000(OCR) = 500us
    TCCR3A = 0x00; // CTC mode + turn timer3 off
    TCCR3B = (1 << WGM32);
    TIMSK3 = (1 << OCIE3A); // enable compare match interrupt
    OCR3A = DB_CS_OCR_MAX;  // set measurement period
    // --------------------------------------------------------------------------

    uart0_msg("----------------------------------------\n");
    uart0_msg("dbCs\n");
    uart0_msg("----------------------------------------\n");
    uart0_msg("timing values    : ");
    for (i = 0; i < DB_CS_DATA_SIZE; i++)
    {
        _dbCs_ocrValues[i] = eeprom_read16(DB_CS_EEPROM_ADDRESS + i * 2);
        sprintf(text, "%d ", _dbCs_ocrValues[i]);
        uart0_msg(text);
    }
    uart0_msg("\n");

    _dbCs_colorNo = eeprom_read(DB_CS_EEPROM_ADDRESS + 12 * 2);
    sprintf(text, "colors registered: %d\n", _dbCs_colorNo);
    uart0_msg(text);
    for (i = 0; i < _dbCs_colorNo && i < DB_CS_MAX_COLORS; i++)
    {
        sprintf(text, "color %d          : ", i);
        uart0_msg(text);
        for (j = 0; j < 12; j++)
        {
            _dbCs_colorRefs[i][j] = eeprom_read(DB_CS_EEPROM_ADDRESS + 12 * 2 + 1 + i * 12 + j);
            sprintf(text, "%2d ", _dbCs_colorRefs[i][j]);
            uart0_msg(text);
        }
        uart0_msg("\n");
    }
    uart0_msg("----------------------------------------\n\n");

    if (dbBtn_isPressed(DB_BTN_BLUE))
    {
        _dbCs_registrationFinished = 0;

        dbLs_playSong(dbLs_beepBeep, DB_LS_REGULAR, NULL);
        dbLed_prepareColor(DB_LED_BUTTON_RED, 0, 0, 0);
        dbLed_prepareColor(DB_LED_BUTTON_GREEN, 0, 0, 0);
        dbLed_prepareColor(DB_LED_BUTTON_BLUE, 0, 0, 255);
        dbLed_refresh();
        while (dbBtn_isPressed(DB_BTN_BLUE))
            ;

        dbLed_prepareColor(DB_LED_BUTTON_RED, 255, 0, 0);
        dbLed_prepareColor(DB_LED_BUTTON_GREEN, 0, 0, 0);
        dbLed_prepareColor(DB_LED_BUTTON_BLUE, 0, 0, 255);
        dbLed_refresh();
        dbCs_calibrate(NULL);

        dbBtn_registerBlueCallback(_dbCs_bluePressed);
        dbBtn_registerRedCallback(_dbCs_redPressed);
        while (!_dbCs_registrationFinished)
            ;

        dbLs_playSong(dbLs_beepBeep, DB_LS_REGULAR, NULL);
        dbLed_prepareColor(DB_LED_BUTTON_RED, 0, 0, 0);
        dbLed_prepareColor(DB_LED_BUTTON_GREEN, 0, 0, 0);
        dbLed_prepareColor(DB_LED_BUTTON_BLUE, 0, 0, 0);
        dbLed_refresh();
        dbBtn_registerBlueCallback(NULL);
    }
    sei();
}

uint8_t dbCs_isInitialized()
{
    return _dbCs_initialized;
}

uint8_t _dbCs_getColorDiff(const struct DbCsColor *refColor, const struct DbCsColor *color)
{
    int16_t diff = (int16_t)(refColor->r - color->r) * (int16_t)(refColor->r - color->r) + (int16_t)(refColor->g - color->g) * (int16_t)(refColor->g - color->g) + (int16_t)(refColor->b - color->b) * (int16_t)(refColor->b - color->b);
    if (diff > 255)
        diff = 255;
    return (uint8_t)diff;
}

uint8_t _dbCs_updateColorIndexes()
{
    int16_t diff;
    uint8_t changed = 0;
    for (uint8_t sensor = 0; sensor < 3; sensor++)
    {
        ((int8_t *)&_dbCs_colorIndexes)[sensor] = -1;
        ((uint8_t *)&_dbCs_colorIndexesQuality)[sensor] = -1;

        for (uint8_t refColor = 0; refColor < _dbCs_colorNo; refColor++)
        {
            diff = _dbCs_getColorDiff(&((const struct DbCsColor *)_dbCs_colorRefs[refColor])[sensor], &(((const struct DbCsColor *)&_dbCs_values)[sensor]));
            if (diff < ((uint8_t *)&_dbCs_colorIndexesQuality)[sensor])
            {
                ((int8_t *)&_dbCs_colorIndexes)[sensor] = refColor;
                ((uint8_t *)&_dbCs_colorIndexesQuality)[sensor] = diff;
            }
        }
        if (((int8_t *)&_dbCs_colorIndexes)[sensor] != ((int8_t *)&_dbCs_lastColorIndexes)[sensor])
        {
            if (((uint8_t *)&_dbCs_colorIndexesQuality)[sensor] <= _dbCs_minColorIndexesQuality)
            {
                ((int8_t *)&_dbCs_lastColorIndexes)[sensor] = ((int8_t *)&_dbCs_colorIndexes)[sensor];
                changed = 1;
            }
            else
            {
                ((int8_t *)&_dbCs_colorIndexes)[sensor] = ((int8_t *)&_dbCs_lastColorIndexes)[sensor];
            }
        }
    }

    return changed;
}

ISR(TIMER0_COMPA_vect)
{
    _dbCs_overflow = 1;
}

ISR(TIMER3_COMPA_vect)
{
    uint8_t i;

    if (_dbCs_mode == DB_CS_CALIBRATE)
    {
        if (_dbCs_overflow) // measure period too long
        {
            OCR3A -= DB_CS_OCR_STEP;
        }
        else
        {
            _dbCs_ocrValues[_dbCs_sensor * 4 + _dbCs_colorChannel] = OCR3A;
            OCR3A = DB_CS_OCR_MAX;
            if (++_dbCs_colorChannel == 4)
            {
                _dbCs_colorChannel = 0;
                if (++_dbCs_sensor == 3)
                {
                    _dbCs_whiteLeds(0);
                    TCCR0B &= ~((1 << CS02) | (1 << CS01)); // disable counter
                    TCCR3B &= ~(1 << CS31);                 // disable timer

                    for (i = 0; i < DB_CS_DATA_SIZE; i++)
                    {
                        eeprom_write16(DB_CS_EEPROM_ADDRESS + i * 2, _dbCs_ocrValues[i]);
                    }
                    // reset the number of colors registered
                    eeprom_write(DB_CS_EEPROM_ADDRESS + 12 * 2, 0);
                    _dbCs_colorNo = 0;

                    if (_dbCs_calibratedCallback)
                        (*_dbCs_calibratedCallback)((uint16_t *)_dbCs_ocrValues);
                }
            }
            _dbCs_setColor(_dbCs_colorChannel);
            _dbCs_setSensor(_dbCs_sensor);
        }
    }
    else
    {
        _dbCs_values[_dbCs_sensor * 4 + _dbCs_colorChannel] = (_dbCs_overflow == 0) ? TCNT0 : 15;

        // sometimes it happens that TCNT0 gets values bigger than 15 - have no glue why!
        if (_dbCs_values[_dbCs_sensor * 4 + _dbCs_colorChannel] > 15)
        {
            _dbCs_values[_dbCs_sensor * 4 + _dbCs_colorChannel] = 15;
        }
        if (++_dbCs_colorChannel == 4)
        {
            _dbCs_colorChannel = 0;
            if (++_dbCs_sensor == 3)
            {
                _dbCs_whiteLeds(0);
                TCCR0B &= ~((1 << CS02) | (1 << CS01)); // disable counter
                TCCR3B &= ~(1 << CS31);                 // disable timer

                if (_dbCs_mode == DB_CS_REGISTER)
                {
                    for (i = 0; i < DB_CS_DATA_SIZE; i++)
                    {
                        _dbCs_colorRefs[_dbCs_colorNo][i] = _dbCs_values[i];
                        eeprom_write(DB_CS_EEPROM_ADDRESS + DB_CS_DATA_SIZE * 2 + 1 + _dbCs_colorNo * DB_CS_DATA_SIZE + i, _dbCs_values[i]);

                        //char text[32];
                        //sprintf(text, "%d ", _dbCs_values[i]);
                        //uart0_msg(text);
                    }
                    _dbCs_colorNo++;
                    eeprom_write(DB_CS_EEPROM_ADDRESS + DB_CS_DATA_SIZE * 2, _dbCs_colorNo);
                    if (_dbCs_registeredCallback)
                        (*_dbCs_registeredCallback)(_dbCs_colorNo - 1, (const struct DbCsColors *)_dbCs_values);
                }
                else
                {
                    if (_dbCs_getColorsCallback)
                        (*_dbCs_getColorsCallback)((const struct DbCsColors *)_dbCs_values);
                    else if (_dbCs_getColorIndexesCallback || _dbCs_colorIndexesChangedCallback)
                    {
                        if (_dbCs_updateColorIndexes() && _dbCs_colorIndexesChangedCallback)
                        {
                            _dbCs_colorIndexesChangedCallback((const struct DbCsColors *)_dbCs_values, (const struct DbCsColorIndexes *)&_dbCs_colorIndexes, (const struct DbCsColorIndexesQuality *)&_dbCs_colorIndexesQuality);
                        }
                        if (_dbCs_getColorIndexesCallback)
                        {
                            _dbCs_getColorIndexesCallback((const struct DbCsColors *)_dbCs_values, (const struct DbCsColorIndexes *)&_dbCs_colorIndexes, (const struct DbCsColorIndexesQuality *)&_dbCs_colorIndexesQuality);
                        }
                    }
                }
            }
        }
        _dbCs_setColor(_dbCs_colorChannel);
        _dbCs_setSensor(_dbCs_sensor);
        OCR3A = _dbCs_ocrValues[_dbCs_sensor * 4 + _dbCs_colorChannel];
    }
    TCNT0 = 0;
    TCNT3 = 0;
    _dbCs_overflow = 0;
}

void _dbCs_trigger(enum DbCsMode mode)
{
    _dbCs_mode = mode;
    _dbCs_colorChannel = 0;
    _dbCs_sensor = 0;

    _dbCs_whiteLeds(1);

    _dbCs_setSensor(_dbCs_sensor);
    _dbCs_setColor(_dbCs_colorChannel);

    switch (mode)
    {
    case DB_CS_CALIBRATE:
    {
        OCR3A = DB_CS_OCR_MAX;
        break;
    }
    default:
    {
        OCR3A = _dbCs_ocrValues[_dbCs_sensor * 4 + _dbCs_colorChannel];
    }
    }
    TCNT3 = 0;
    TCNT0 = 0;
    _dbCs_overflow = 0;

    TCCR0B = ((1 << CS02) | (1 << CS01)); // enable counter; count falling edges
    TCCR3B |= (1 << CS31);                // enable timer
}

void dbCs_calibrate(void (*readyCallback)(uint16_t calibrationData[DB_CS_DATA_SIZE]))
{
    _dbCs_calibratedCallback = readyCallback;
    _dbCs_trigger(DB_CS_CALIBRATE);
}

void dbCs_getColors(void (*readyCallback)(const struct DbCsColors *colors))
{
    _dbCs_getColorsCallback = readyCallback;
    _dbCs_getColorIndexesCallback = NULL;
    _dbCs_trigger(DB_CS_MEASURE);
}

void dbCs_getColorIndexes(void (*readyCallback)(const struct DbCsColors *colors, const struct DbCsColorIndexes *colorIndexes, const struct DbCsColorIndexesQuality *colorIndexesQuality), void (*changedCallback)(const struct DbCsColors *colors, const struct DbCsColorIndexes *colorIndexes, const struct DbCsColorIndexesQuality *colorIndexesQuality))
{
    if (!_dbCs_initialized)
    {
        uart0_msg("dbCs_getColorIndexes: dbCs_init missing\n");
        return;
    }

    _dbCs_getColorsCallback = NULL;
    _dbCs_getColorIndexesCallback = readyCallback;
    _dbCs_colorIndexesChangedCallback = changedCallback;
    _dbCs_trigger(DB_CS_MEASURE);
}

uint8_t dbCs_registerColor(void (*readyCallback)(int8_t colorIndex, const struct DbCsColors *colors))
{
    if (!_dbCs_initialized)
    {
        uart0_msg("dbCs_registerColor: dbCs_init missing\n");
        return 0;
    }

    if (_dbCs_colorNo < DB_CS_MAX_COLORS)
    {
        _dbCs_registeredCallback = readyCallback;
        _dbCs_trigger(DB_CS_REGISTER);
        return 1;
    }
    return 0;
}
uint16_t _dbCs_continuousMeasurements()
{
    dbCs_getColorIndexes(NULL, _dbCs_colorIndexesChangedCallback);
    return _dbCs_retriggerTime_ms;
}

void dbCs_startContinuousMeasurements(uint16_t time_ms, void (*changedCallback)(const struct DbCsColors *colors, const struct DbCsColorIndexes *colorIndexes, const struct DbCsColorIndexesQuality *colorIndexesQuality))
{
    if (!_dbCs_initialized)
    {
        uart0_msg("dbCs_startContinuousMeasurements: dbCs_init missing\n");
        return;
    }

    if (time_ms < 10)
        time_ms = 10; // minimal period time

    _dbCs_getColorIndexesCallback = NULL;
    _dbCs_colorIndexesChangedCallback = changedCallback;
    _dbCs_retriggerTime_ms = time_ms;

    if (_dbCs_measureHandle)
    {
        tb_unregister(_dbCs_measureHandle);
    }

    _dbCs_lastColorIndexes.left = -1; // to trigger at least one changed callback
    _dbCs_measureHandle = tb_register(_dbCs_continuousMeasurements, time_ms);
    if (!_dbCs_measureHandle)
    {
        uart0_msg("dbCs_startContinuousMeasurements: could not register tb-callback\n");
        return;
    }
}

void dbCs_stopContinuousMeasurements()
{
    if (!_dbCs_initialized)
    {
        uart0_msg("dbCs_stopContinuousMeasurements: dbCs_init missing\n");
        return;
    }

    if (_dbCs_measureHandle)
    {
        tb_unregister(_dbCs_measureHandle);
        _dbCs_measureHandle = 0;
    }
}

void dbCs_setMinColorIndexesQuality(uint8_t minQuality)
{
    _dbCs_minColorIndexesQuality = minQuality;
}

uint8_t dbCs_getNumberOfColors()
{
    return _dbCs_colorNo;
}

uint8_t dbCs_getColorOfIndex(uint8_t index, const struct DbCsColors **colors)
{
    if (index >= _dbCs_colorNo)
    {
        return 0;
    }
    *colors = ((const struct DbCsColors *)_dbCs_colorRefs[index]);
    return 1;
}

uint8_t dbCs_doesContinuouslyMeasure()
{
    return (_dbCs_measureHandle != 0);
}
