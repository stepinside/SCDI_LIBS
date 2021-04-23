
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tb.h"
#include <uart.h>

enum CallbackState
{
  TB_FREE = 0,
  TB_ACTIVE = 1,
  TB_REGISTER = 2,
  TB_UNREGISTER = 3,
};

struct TimeBaseCallbackInfo
{
  enum CallbackState state;
  uint16_t ticks;
  uint16_t actTick;
  uint8_t running;
  uint16_t (*callback)();
};

static struct TimeBaseCallbackInfo *_tbCallbackInfo = NULL;
static uint8_t _tbCallbackNo = 0;
static uint8_t _tbMaxCallbackNo = 0;
static uint16_t _tbBaseTime_ms = 0;
static uint32_t _tbActTime_ms = 0;

static uint8_t _tb_initialized = 0;

void tb_debug()
{
  uint8_t i;
  struct TimeBaseCallbackInfo *tbPtr = _tbCallbackInfo;
  static char text[64];

  uart0_msg("tb-status\n");
  for (i = 0; i < _tbMaxCallbackNo; i++)
  {
    if (tbPtr->state != TB_FREE)
    {
      sprintf(text, "tb: %02d, %d, %04x, %d\n", i + 1, tbPtr->state, tbPtr->callback, tbPtr->ticks);
      uart0_msg(text);
    }
    tbPtr++;
  }
  uart0_msg("\n");
}

uint8_t tb_init(enum TB_BaseTime baseTime_ms, uint8_t maxCallbackNo)
{
  uint8_t i;
  struct TimeBaseCallbackInfo *tbPtr;

  if (_tb_initialized)
  {
    uart0_msg("tb_init: already initialized\n");
    return 0;
  }

  _tbBaseTime_ms = (uint16_t)baseTime_ms;
  _tbActTime_ms = 0;

  // 62.5ns * 160000 = 10ms
  // 62.5ns * 256(PS) * 625 = 10ms
  if (_tbCallbackInfo != NULL)
  {
    free(_tbCallbackInfo);
    _tbCallbackInfo = NULL;
  }

  _tbCallbackNo = 0;
  _tbMaxCallbackNo = 0;

  _tbCallbackInfo = (struct TimeBaseCallbackInfo *)malloc(sizeof(struct TimeBaseCallbackInfo) * maxCallbackNo);
  if (_tbCallbackInfo != NULL)
  {
    TCCR1A = 0x00;
    TCCR1B = (1 << WGM12) | (1 << CS12); // CTC mode; PS=256
    OCR1A = 625 * (uint16_t)(baseTime_ms / 10);
    TIMSK1 = (1 << OCIE1A);

    _tbMaxCallbackNo = maxCallbackNo;

    tbPtr = _tbCallbackInfo;
    for (i = 0; i < maxCallbackNo; i++)
    {
      tbPtr->state = TB_FREE;
      tbPtr++;
    }
    sei();
  }
  else
  {
    TCCR1A = 0x00; // turn timer off
    TCCR1B = 0x00;
    uart0_msg("tb_init: too less memory\n");

    return 0;
  }
  _tb_initialized = 1;
  return 1;
}

uint8_t tb_isInitialized()
{
  return _tb_initialized;
}

uint16_t tb_getBaseTime_ms()
{
  if (!_tb_initialized)
  {
    uart0_msg("tb_getBaseTime_ms: tb_init missing\n");
    return 0;
  }
  return _tbBaseTime_ms;
}

uint32_t tb_getTime_ms()
{
  return _tbActTime_ms;
}

ISR(TIMER1_COMPA_vect)
{
  uint8_t i, callbacksFound = 0;
  struct TimeBaseCallbackInfo *tbPtr;

  _tbActTime_ms += _tbBaseTime_ms;

  // free timers that shall be freed
  for (i = 0, tbPtr = _tbCallbackInfo; i < _tbMaxCallbackNo; i++)
  {
    if (tbPtr->state == TB_REGISTER)
      tbPtr->state = TB_ACTIVE;
    else if (tbPtr->state == TB_UNREGISTER)
      tbPtr->state = TB_FREE;
    tbPtr++;
  }

  for (i = 0, tbPtr = _tbCallbackInfo; (callbacksFound < _tbCallbackNo) && (i < _tbMaxCallbackNo); i++)
  {
    if (tbPtr->state == TB_ACTIVE)
    {
      callbacksFound++;
      if (tbPtr->running)
      {
        tbPtr->actTick++;
        if (tbPtr->actTick >= tbPtr->ticks)
        {
          tbPtr->actTick = 0;
          tbPtr->ticks = tbPtr->callback() / _tbBaseTime_ms;
          if (!tbPtr->ticks)
          {
            tbPtr->state = TB_UNREGISTER;
            _tbCallbackNo--;
            callbacksFound--; // bug corrected -> 25.6.2018
          }
        }
      }
    }
    tbPtr++;
  }
}

uint8_t tb_register(uint16_t (*callback)(), uint16_t time_ms)
{
  uint8_t i;
  struct TimeBaseCallbackInfo *tbPtr = _tbCallbackInfo;

  if (!_tb_initialized)
  {
    uart0_msg("tb_register: tb_init missing\n");
    return 0;
  }

  uint8_t bit = bit_is_set(SREG, 7);
  if (bit)
    cli();

  for (i = 0; i < _tbMaxCallbackNo; i++)
  {
    if (tbPtr->state == TB_FREE)
    {
      tbPtr->callback = callback;
      tbPtr->running = 1;
      tbPtr->ticks = time_ms / _tbBaseTime_ms;
      if (!tbPtr->ticks || time_ms % _tbBaseTime_ms)
      {
        uart0_msg("tb_register: invalid time_ms - must be a multiple of the basetime\n");
        if (bit)
          sei();
        return 0;
      }
      tbPtr->state = TB_REGISTER;
      tbPtr->actTick = 0;
      _tbCallbackNo++;
      if (bit)
        sei();
      return i + 1;
    }
    tbPtr++;
  }
  uart0_msg("tb_register: cannot register callback; increase maxCallbacks in tb_init\n");
  if (bit)
    sei();
  return 0;
}

uint8_t tb_unregister(uint8_t handle)
{
  uint8_t bit = bit_is_set(SREG, 7);

  if (!_tb_initialized)
  {
    uart0_msg("tb_unregister: tb_init missing\n");
    return 0;
  }

  if (bit)
    cli();
  handle--;
  if (handle < _tbMaxCallbackNo && _tbCallbackInfo[handle].ticks)
  {
    _tbCallbackInfo[handle].state = TB_UNREGISTER;
    _tbCallbackNo--;
    if (bit)
      sei();
    return 1;
  }

  uart0_msg("tb_unregister: invalid handle\n");
  if (bit)
    sei();
  return 0;
}

uint8_t tb_resetTimeout(uint8_t handle)
{
  if (!_tb_initialized)
  {
    uart0_msg("tb_resetTimeout: tb_init missing\n");
    return 0;
  }

  handle--;
  if (handle < _tbCallbackNo)
  {
    _tbCallbackInfo[handle].actTick = 0;
    return 1;
  }
  uart0_msg("tb_resetTimeout: invalid handle\n");
  return 0;
}

uint8_t tb_stopTimeout(uint8_t handle)
{
  if (!_tb_initialized)
  {
    uart0_msg("tb_stopTimeout: tb_init missing\n");
    return 0;
  }

  handle--;
  if (handle < _tbCallbackNo)
  {
    _tbCallbackInfo[handle].running = 0;
    return 1;
  }
  uart0_msg("tb_stopTimeout: invalid handle\n");
  return 0;
}

uint8_t tb_startTimeout(uint8_t handle)
{
  if (!_tb_initialized)
  {
    uart0_msg("tb_startTimeout: tb_init missing\n");
    return 0;
  }

  handle--;
  if (handle < _tbCallbackNo)
  {
    _tbCallbackInfo[handle].running = 1;
    return 1;
  }
  uart0_msg("tb_startTimeout: invalid handle\n");
  return 0;
}
