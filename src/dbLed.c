#include <avr/io.h>
#include <stdio.h>
#ifndef F_CPU
#define F_CPU 16000000
#endif
#include <util/delay.h>

#include <ws2812.h>
#include <uart.h>

#include "dbLed.h"

static Ws2812Display *_dbLed_display = NULL;
static uint8_t _dbLed_initialized = 0;

void dbLed_init()
{
    if (_dbLed_initialized)
        return;

    _dbLed_initialized = 1;

    if (_dbLed_display)
    {
        ws2812_delete(_dbLed_display);
    }

    _dbLed_display = ws2812_create(11, 1);
    for (uint8_t i = DB_LED_FRONT_RIGHT_OUT; i <= DB_LED_BACK_RIGHT_OUT; i++)
    {
        ws2812_setPixel(_dbLed_display, i, 0, 0, 0, 0);
    }
    ws2812_refresh(_dbLed_display, &PORTB, 7);
}

uint8_t dbLed_isInitialized()
{
    return _dbLed_initialized;
}

void dbLed_prepareColor(enum DB_LED_ID ledId, uint8_t r, uint8_t g, uint8_t b)
{
    if (!_dbLed_initialized)
    {
        uart0_msg("dbLed_prepareColor: dbLed_init missing\n");
        return;
    }

    if ((ledId >= DB_LED_FRONT_RIGHT_OUT) && (ledId <= DB_LED_BACK_RIGHT_OUT))
    {
        ws2812_setPixel(_dbLed_display, ledId, 0, r, g, b);
    }
}

void dbLed_refresh()
{
    if (!_dbLed_initialized)
    {
        uart0_msg("dbLed_refresh: dbLed_init missing\n");
        return;
    }

    ws2812_refresh(_dbLed_display, &PORTB, 7);
    _delay_ms(5);
}

void dbLed_setColor(enum DB_LED_ID ledId, uint8_t r, uint8_t g, uint8_t b)
{
    if (!_dbLed_initialized)
    {
        uart0_msg("dbLed_setColor: dbLed_init missing\n");
        return;
    }

    if ((ledId >= DB_LED_FRONT_RIGHT_OUT) && (ledId <= DB_LED_BACK_RIGHT_OUT))
    {
        ws2812_setPixel(_dbLed_display, ledId, 0, r, g, b);
    }
    dbLed_refresh();
}

void updateLed(enum DB_LED_ID ledId, uint8_t index)
{
    uint8_t r, g, b;

    switch (index)
    {
    case 0:
        r = 0xff, g = 0xff, b = 0xff;
        break;
    case 1:
        r = 0xff, g = 0xff, b = 0x00;
        break;
    case 2:
        r = 0xff, g = 0x00, b = 0x00;
        break;
    case 3:
        r = 0x00, g = 0xff;
        b = 0x00;
        break;
    case 4:
        r = 0x00, g = 0x00, b = 0xff;
        break;
    case 5:
        r = 0x10, g = 0x10, b = 0x10;
        break;
    default:
        r = 0x00, g = 0x00;
        b = 0x00;
        break;
    }
    dbLed_prepareColor(ledId, r, g, b);
}
