#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#ifndef F_CPU
#define F_CPU 16000000
#endif
#include <util/delay.h>

#include <tb.h>
#include <uart.h>

#include "dbIrc.h"

uint8_t _dbIrc_timeout;
uint8_t _dbIrc_cnt = 0;
uint32_t _dbIrc_value = 0;
uint8_t _dbIrc_initialized = 0;
void (*_dbIrc_callback)(uint32_t code) = NULL;

void dbIrc_registerReceptionCallback(void(*callback)(uint32_t code))
{
  if (!_dbIrc_initialized)
  {
    uart0_msg("dbIrc_registerReceptionCallback: dbIrc_init missing\n");
  }
  _dbIrc_callback = callback;
}

uint16_t _dbIrc_reset()
{
  _dbIrc_cnt = 0;
  _dbIrc_value = 0;
  tb_stopTimeout(_dbIrc_timeout);
  return 200;
}

ISR(INT2_vect)
{
  static uint16_t lastValue = 0;
  uint8_t value;

  int16_t diff = TCNT1 - lastValue;
  lastValue = TCNT1;

  if (diff < 0)
  {
    diff += OCR1A;      // an overflow happened
  }
  if (diff >= 0 && diff <= 100)
  {
    value = 1;
  }
  else
  {
    value = 0;
  }

  if (_dbIrc_cnt == 0)
  {
    tb_startTimeout(_dbIrc_timeout);
    _dbIrc_value = 0;
    _dbIrc_cnt++;
  }
  else if (_dbIrc_cnt == 1)
  {
    // ignore bit 1
    _dbIrc_cnt++;
  }
  else if (_dbIrc_cnt == 34)
  {
    // ignore bit 34
    _dbIrc_cnt++;
  }
  else if (_dbIrc_cnt == 35)
  {
    tb_stopTimeout(_dbIrc_timeout);
    if (_dbIrc_callback)
    {
      (*_dbIrc_callback)(_dbIrc_value);
    }
    _dbIrc_cnt = 0;
  }
  else if (_dbIrc_cnt >= 2 && _dbIrc_cnt <= 33)
  {
    if (value)
    {
      _dbIrc_value |= (0x80000000 >> (_dbIrc_cnt-2));
    }
    _dbIrc_cnt++;
  }
  tb_resetTimeout(_dbIrc_timeout);
}

void dbIrc_init()
{
  if (_dbIrc_initialized)   return;
  _dbIrc_initialized = 1;

  DDRD |= (1 << 3);       // infrared sender (=diode) requires an output pin
  PORTD &= ~(1 << 3);     // set sender pin to 0

  DDRD &= ~(1 << 2);
  PORTD |= (1 << 2);

  EICRA |= (1 << ISC21);
  EICRA &= ~(1 << ISC20);

  EIMSK |= (1 << INT2);

  _dbIrc_timeout = tb_register(_dbIrc_reset, 200);
  if (!_dbIrc_timeout)
  {
    uart0_msg("dbIrc_init: could not register tb-callback\n");
  }
  tb_stopTimeout(_dbIrc_timeout);

  sei();
}

uint8_t dbIrc_isInitialized()
{
  return _dbIrc_initialized;
}

void _dbIrc_toggle(uint16_t periodes)
{
  uint16_t len;

  for (len=0; len<periodes; len++)
  {
    PORTD |= (1 << 3);
    _delay_us(13);
    PORTD &= ~(1 << 3);
    _delay_us(13);
  }
}

#define DB_IRC_BIT          560 * 38 / 1000
#define DB_IRC_ZERO_SPACE   560
#define DB_IRC_ONE_SPACE    1690
#define DB_IRC_HDR_MARK     9000L * 38 / 1000  // ???
#define DB_IRC_HDR_SPACE    4500

void dbIrc_send(uint32_t code)
{
  if (!_dbIrc_initialized)
  {
    uart0_msg("dbIrc_registerReceptionCallback: dbIrc_init missing\n");
    return;
  }

  uint8_t oldSREG = SREG;
  cli();

  _dbIrc_toggle(DB_IRC_HDR_MARK);
  _delay_us(DB_IRC_HDR_SPACE);

  for (uint8_t pos=0; pos<32; pos++)
  {
    if (code & (0x80000000 >> pos))
    {
      _dbIrc_toggle(DB_IRC_BIT);
      _delay_us(DB_IRC_ZERO_SPACE);
    }
    else
    {
      _dbIrc_toggle(DB_IRC_BIT);
      _delay_us(DB_IRC_ONE_SPACE);
    }
  }

  _dbIrc_toggle(DB_IRC_BIT);
  _delay_us(DB_IRC_ONE_SPACE);
  _dbIrc_toggle(DB_IRC_BIT);
  _delay_us(DB_IRC_ONE_SPACE);
  _dbIrc_toggle(DB_IRC_BIT);
  _delay_us(DB_IRC_ONE_SPACE);

  SREG = oldSREG;
}

uint8_t dbIrc_received(uint32_t command)
{
  if (_dbIrc_callback)
  {
    (*_dbIrc_callback)(command);
    return 1;
  }
  return 0;
}

uint8_t dbIrc_callbackRegistered()
{
  return (_dbIrc_callback != NULL);
}