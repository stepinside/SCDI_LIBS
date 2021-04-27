#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#ifndef F_CPU
#define F_CPU 16000000
#endif
#include <util/delay.h>

#include <tb.h>
#include <dbLed.h>
#include "dbBtn.h"

#include <uart.h>

static uint8_t _dbBtn_lastState;
static void (*_dbBtn_redCallback)(uint8_t state) = NULL;
static void (*_dbBtn_greenCallback)(uint8_t state) = NULL;
static void (*_dbBtn_blueCallback)(uint8_t state) = NULL;

static uint8_t _dbBtn_initialized = 0;

void dbBtn_init()
{
    if (!tb_isInitialized())
    {
        uart0_msg("dbBtn_init: the timebase must be initialized before by tb_init\n");
        return;
    }

    if (_dbBtn_initialized)
        return;

    _dbBtn_initialized = 1;

    DDRB &= ~(DB_BTN_RED | DB_BTN_GREEN | DB_BTN_BLUE);
    PORTB |= (DB_BTN_RED | DB_BTN_GREEN | DB_BTN_BLUE);

    // in order to get stable conditions wait 10ms
    // otherwise checking the state of PINB might fail
    _delay_ms(10);

    PCMSK0 |= ((1 << PCINT4) | (1 << PCINT5) | (1 << PCINT6));
    PCICR |= (1 << PCIE0);
    sei();

    _dbBtn_lastState = (PINB & (DB_BTN_RED | DB_BTN_GREEN | DB_BTN_BLUE));
}

uint8_t dbBtn_isInitialized()
{
    return _dbBtn_initialized;
}

uint16_t dbBtn_debounceTimeOver()
{
    PCICR |= (1 << PCIE0);
    return 0;
}

uint8_t dbBtn_isPressed(enum DbBtn button)
{
    return !(PINB & button);
};

ISR(PCINT0_vect)
{
    uint8_t currentState = (PINB & (DB_BTN_RED | DB_BTN_GREEN | DB_BTN_BLUE));
    uint8_t changes = currentState ^ _dbBtn_lastState;

    if ((changes & DB_BTN_RED) && _dbBtn_redCallback)
    {
        _dbBtn_redCallback((currentState & DB_BTN_RED) == 0);
    }
    if ((changes & DB_BTN_GREEN) && _dbBtn_greenCallback)
    {
        _dbBtn_greenCallback((currentState & DB_BTN_GREEN) == 0);
    }
    if ((changes & DB_BTN_BLUE) && _dbBtn_blueCallback)
    {
        _dbBtn_blueCallback((currentState & DB_BTN_BLUE) == 0);
    }

    // if the state of a button changed
    if (changes & (DB_BTN_RED | DB_BTN_GREEN | DB_BTN_BLUE))
    {
        // disable button interrupts for 100ms and then reevaluate the state
        PCICR &= ~(1 << PCIE0);
        tb_register(dbBtn_debounceTimeOver, 100);
    }

    _dbBtn_lastState = currentState;
}

void dbBtn_registerRedCallback(void (*callback)(uint8_t state))
{
    if (!_dbBtn_initialized)
    {
        uart0_msg("dbBtn_registerRedCallback: dbBtn_init missing\n");
    }
    _dbBtn_redCallback = callback;
}
void dbBtn_registerGreenCallback(void (*callback)(uint8_t state))
{
    if (!_dbBtn_initialized)
    {
        uart0_msg("dbBtn_registerGreenCallback: dbBtn_init missing\n");
    }
    _dbBtn_greenCallback = callback;
}
void dbBtn_registerBlueCallback(void (*callback)(uint8_t state))
{
    if (!_dbBtn_initialized)
    {
        uart0_msg("dbBtn_registerBlueCallback: dbBtn_init missing\n");
    }
    _dbBtn_blueCallback = callback;
}
