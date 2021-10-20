#include <avr/io.h>

#include <ws2812.h>
#include <tb.h>
#include <uart.h>

#include <dbLed.h>
//#include <dbLs.h>
#include "dbLedCar.h"

static uint8_t _dbLedCar_state = 0;
static uint8_t _dbLedCar_indicator = DB_LED_CAR_INDICATOR_OFF;
static uint8_t _dbLedCar_indicatorLight = 0;

static uint8_t _dbLedCar_indicatorHandle = 0;

static uint8_t _dbLedCar_initialized = 0;

void dbLedCar_init()
{
    _dbLedCar_initialized = 1;
    dbLed_init();
}

uint8_t dbLedCar_isInitialized()
{
    return _dbLedCar_initialized;
}

uint8_t _dbLedCar_indicatorSound()
{
    return 0;
}

uint16_t _dbLedCar_indicatorBlink()
{
    //  static struct DbLsNote blinkSound[] = {{DB_LS_C3,DB_LS_1_4}, {DB_LS_PAUSE, DB_LS_1_4}, {DB_LS_LAST, DB_LS_1_1}};

    _dbLedCar_indicatorLight ^= 1;
    if (_dbLedCar_indicatorLight)
    {
        //    dbLs_playSong(blinkSound, DB_LS_STACCATO1, _dbLedCar_indicatorSound);
    }
    dbLedCar_update();
    return 400;
}

void dbLedCar_update()
{
    dbLed_prepareColor(DB_LED_FRONT_RIGHT_OUT, 0, 0, 0);
    dbLed_prepareColor(DB_LED_FRONT_RIGHT_IN, 0, 0, 0);
    dbLed_prepareColor(DB_LED_FRONT_LEFT_IN, 0, 0, 0);
    dbLed_prepareColor(DB_LED_FRONT_LEFT_OUT, 0, 0, 0);
    dbLed_prepareColor(DB_LED_BACK_RIGHT_OUT, 0, 0, 0);
    dbLed_prepareColor(DB_LED_BACK_RIGHT_IN, 0, 0, 0);
    dbLed_prepareColor(DB_LED_BACK_LEFT_IN, 0, 0, 0);
    dbLed_prepareColor(DB_LED_BACK_LEFT_OUT, 0, 0, 0);

    if (_dbLedCar_state & DB_LED_CAR_LIGHT)
    {
        dbLed_prepareColor(DB_LED_FRONT_RIGHT_OUT, 0, 0, 0);
        dbLed_prepareColor(DB_LED_FRONT_RIGHT_IN, 64, 64, 64);
        dbLed_prepareColor(DB_LED_FRONT_LEFT_IN, 64, 64, 64);
        dbLed_prepareColor(DB_LED_FRONT_LEFT_OUT, 0, 0, 0);
        dbLed_prepareColor(DB_LED_BACK_RIGHT_OUT, 0, 0, 0);
        dbLed_prepareColor(DB_LED_BACK_RIGHT_IN, 64, 0, 0);
        dbLed_prepareColor(DB_LED_BACK_LEFT_IN, 64, 0, 0);
        dbLed_prepareColor(DB_LED_BACK_LEFT_OUT, 0, 0, 0);
    }

    if (_dbLedCar_state & DB_LED_CAR_HEADLIGHT)
    {
        dbLed_prepareColor(DB_LED_FRONT_RIGHT_OUT, 255, 255, 255);
        dbLed_prepareColor(DB_LED_FRONT_RIGHT_IN, 255, 255, 255);
        dbLed_prepareColor(DB_LED_FRONT_LEFT_IN, 255, 255, 255);
        dbLed_prepareColor(DB_LED_FRONT_LEFT_OUT, 255, 255, 255);
    }

    if (_dbLedCar_state & DB_LED_CAR_BACKLIGHT)
    {
        dbLed_prepareColor(DB_LED_BACK_RIGHT_IN, 255, 255, 255);
        dbLed_prepareColor(DB_LED_BACK_LEFT_IN, 255, 255, 255);
    }

    if (_dbLedCar_state & DB_LED_CAR_BRAKELIGHT)
    {
        dbLed_prepareColor(DB_LED_BACK_RIGHT_OUT, 255, 0, 0);
        dbLed_prepareColor(DB_LED_BACK_RIGHT_IN, 255, 0, 0);
        dbLed_prepareColor(DB_LED_BACK_LEFT_IN, 255, 0, 0);
        dbLed_prepareColor(DB_LED_BACK_LEFT_OUT, 255, 0, 0);
    }

    if (_dbLedCar_indicatorLight)
    {
        if (_dbLedCar_indicator == DB_LED_CAR_INDICATOR_LEFT)
        {
            dbLed_prepareColor(DB_LED_FRONT_LEFT_OUT, 255, 50, 0);
            dbLed_prepareColor(DB_LED_BACK_LEFT_OUT, 255, 50, 0);
        }
        else if (_dbLedCar_indicator == DB_LED_CAR_INDICATOR_RIGHT)
        {
            dbLed_prepareColor(DB_LED_FRONT_RIGHT_OUT, 255, 50, 0);
            dbLed_prepareColor(DB_LED_BACK_RIGHT_OUT, 255, 50, 0);
        }
        else if (_dbLedCar_indicator == DB_LED_CAR_INDICATOR_BOTH)
        {
            dbLed_prepareColor(DB_LED_FRONT_RIGHT_OUT, 255, 50, 0);
            dbLed_prepareColor(DB_LED_FRONT_LEFT_OUT, 255, 50, 0);
            dbLed_prepareColor(DB_LED_BACK_RIGHT_OUT, 255, 50, 0);
            dbLed_prepareColor(DB_LED_BACK_LEFT_OUT, 255, 50, 0);
        }
    }
    dbLed_refresh();
}

void dbLedCar_light(uint8_t on)
{
    if (!_dbLedCar_initialized)
    {
        uart0_msg("dbLedCar_light: dbLedCar_init missing\n");
        return;
    }

    if (on)
    {
        _dbLedCar_state |= DB_LED_CAR_LIGHT;
    }
    else
    {
        _dbLedCar_state &= ~DB_LED_CAR_LIGHT;
    }

    dbLedCar_update();
}

void dbLedCar_headlight(uint8_t on)
{
    if (!_dbLedCar_initialized)
    {
        uart0_msg("dbLedCar_headlight: dbLedCar_init missing\n");
        return;
    }

    if (on)
    {
        _dbLedCar_state |= DB_LED_CAR_HEADLIGHT;
    }
    else
    {
        _dbLedCar_state &= ~DB_LED_CAR_HEADLIGHT;
    }

    dbLedCar_update();
}

void dbLedCar_brakelight(uint8_t on)
{
    if (!_dbLedCar_initialized)
    {
        uart0_msg("dbLedCar_brakelight: dbLedCar_init missing\n");
        return;
    }

    if (on)
    {
        _dbLedCar_state |= DB_LED_CAR_BRAKELIGHT;
    }
    else
    {
        _dbLedCar_state &= ~DB_LED_CAR_BRAKELIGHT;
    }
    dbLedCar_update();
}

void dbLedCar_backlight(uint8_t on)
{
    if (!_dbLedCar_initialized)
    {
        uart0_msg("dbLedCar_backlight: dbLedCar_init missing\n");
        return;
    }

    if (on)
    {
        _dbLedCar_state |= DB_LED_CAR_BACKLIGHT;
    }
    else
    {
        _dbLedCar_state &= ~DB_LED_CAR_BACKLIGHT;
    }
    dbLedCar_update();
}

void dbLedCar_indicator(enum DbLedCarIndicator indicator)
{
    if (!_dbLedCar_initialized)
    {
        uart0_msg("dbLedCar_indicator: dbLedCar_init missing\n");
        return;
    }

    if (_dbLedCar_indicatorHandle)
    {
        tb_unregister(_dbLedCar_indicatorHandle);
        _dbLedCar_indicatorHandle = 0;
    }

    _dbLedCar_indicator = indicator;
    _dbLedCar_indicatorLight = 0;
    if (_dbLedCar_indicator != DB_LED_CAR_INDICATOR_OFF)
    {
        _dbLedCar_indicatorHandle = tb_register(_dbLedCar_indicatorBlink, 400);
        if (!_dbLedCar_indicatorHandle)
        {
            uart0_msg("dbLedCar_indicator: could not register tb-callback\n");
            return;
        }
    }
    dbLedCar_update();
}

enum DbLedCarIndicator dbLedCar_getIndicator()
{
    return _dbLedCar_indicator;
}

uint8_t dbLedCar_getState()
{
    return _dbLedCar_state;
}
