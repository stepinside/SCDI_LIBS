#include <avr/io.h>
#include <string.h>
#include <stdio.h>

#include <tb.h>
#include <adc.h>
#include <uart.h>

#include "dbIrs.h"

static struct DbDistances _dbIrs_distances;
static volatile uint8_t _dbIrs_sensorIndex;
static volatile uint8_t _dbIrs_sensors;
static volatile uint8_t _dbIrs_handle = 0;
static uint8_t _dbIrs_valuesChanged = 0;
static uint16_t _dbIrs_retriggerTime_ms = 0;

static void (*_dbIrs_readyCallback)(const struct DbDistances *pDistances);
static void (*_dbIrs_changedCallback)(const struct DbDistances *pDistances);

static uint8_t _dbIrs_convert(uint16_t value);
static uint8_t _dbIrs_measured(uint16_t value);
static uint16_t _dbIrs_continuousMeasurement();

static uint8_t _dbIrs_initialized = 0;

struct IrsFactor
{
    uint16_t voltage_mV;
    uint8_t distance_cm;
};

static struct IrsFactor irsFactor[] = {
    // original values taken from the datasheet
    // v1
    //  { 390, 80 }, { 610, 50 }, { 734, 40 }, { 921, 30 },
    //  {1086, 25}, {1328, 20}, {1656, 15}, {2336, 10}, {2750, 8}, {2984, 7}, {3359, 6}, {3500, 5}
    // v1-corrected
    // { 80, 80 }, { 125, 50 }, { 150, 40 }, { 189, 30 },
    // {222, 25}, {272, 20}, {339, 15}, {478, 10}, {563, 8}, {611, 7}, {688, 6}, {717, 5}

    // v2
    {60, 40},
    {88, 30},
    {107, 25},
    {151, 20},
    {165, 16},
    {189, 14},
    {215, 12},
    {257, 10},
    {287, 9},
    {319, 8},
    {360, 7},
    {410, 6},
    {480, 5},
    {559, 4}};

void dbIrs_init()
{
    if (_dbIrs_initialized)
        return;

    _dbIrs_initialized = 1;

    DDRF &= ~0x0f;  // inputs
    PORTF &= ~0x0f; // deactivate pullups

    adc_init();
}

uint8_t dbIrs_isInitialized()
{
    return _dbIrs_initialized;
}

uint8_t _dbIrs_convert(uint16_t value)
{
    uint8_t i = 1;

    while (value > irsFactor[i].voltage_mV)
    {
        i++;
    }

    if (i == 1 && value < irsFactor[0].voltage_mV)
    {
        return 255;
    }

    return (irsFactor[i - 1].distance_cm + (((int16_t)value - (int16_t)irsFactor[i - 1].voltage_mV) * ((int16_t)irsFactor[i].distance_cm - (int16_t)irsFactor[i - 1].distance_cm)) / ((int16_t)irsFactor[i].voltage_mV - (int16_t)irsFactor[i - 1].voltage_mV));
}

// Stops the continuous measurement of distances
void dbIrs_stopContinuousMeasurements()
{
    if (_dbIrs_handle != 0)
    {
        tb_unregister(_dbIrs_handle);
        _dbIrs_handle = 0;
    }
}

uint8_t _dbIrs_measured(uint16_t value)
{
    uint16_t newValue = _dbIrs_convert(value);
    uint8_t *pActDistance = NULL;

    switch (_dbIrs_sensorIndex)
    {
    case 0:
        pActDistance = &(_dbIrs_distances.left_cm);
        break;
    case 1:
        pActDistance = &(_dbIrs_distances.back_cm);
        break;
    case 2:
        pActDistance = &(_dbIrs_distances.right_cm);
        break;
    case 3:
        pActDistance = &(_dbIrs_distances.front_cm);
        break;
    }

    if ((pActDistance != NULL) && (newValue != *pActDistance))
    {
        _dbIrs_valuesChanged = 1;
        *pActDistance = newValue;
    }

    _dbIrs_sensorIndex++;
    while (_dbIrs_sensorIndex < 4 && !(_dbIrs_sensors & (1 << _dbIrs_sensorIndex)))
    {
        _dbIrs_sensorIndex++;
    }

    if (_dbIrs_sensorIndex < 4)
    {
        adc_selectChannel(ADC_AVCC, _dbIrs_sensorIndex);
        return 1;
    }

    if (_dbIrs_readyCallback)
    {
        _dbIrs_readyCallback(&_dbIrs_distances);
    }
    if (_dbIrs_valuesChanged && _dbIrs_changedCallback)
    {
        (*_dbIrs_changedCallback)(&_dbIrs_distances);
    }
    return 0;
}

// stops continuous measurements
void dbIrs_triggerSingleMeasurement(uint8_t sensors, void (*readyCallback)())
{
    if (!_dbIrs_initialized)
    {
        uart0_msg("dbIrs_triggerSingleMeasurement: dbIrs_init missing\n");
        return;
    }

    dbIrs_stopContinuousMeasurements();

    _dbIrs_readyCallback = readyCallback;
    _dbIrs_sensors = (sensors >> 4);

    _dbIrs_sensorIndex = 0;
    while (_dbIrs_sensorIndex < 4 && !(_dbIrs_sensors & (1 << _dbIrs_sensorIndex)))
    {
        _dbIrs_sensorIndex++;
    }

    if (_dbIrs_sensorIndex < 4)
    {
        adc_selectChannel(ADC_AVCC, _dbIrs_sensorIndex);
        adc_trigger10(_dbIrs_measured);
    }
};

uint16_t _dbIrs_continuousMeasurement()
{
    _dbIrs_sensorIndex = 0;
    while (_dbIrs_sensorIndex < 4 && !(_dbIrs_sensors & (1 << _dbIrs_sensorIndex)))
    {
        _dbIrs_sensorIndex++;
    }

    if (_dbIrs_sensorIndex < 4)
    {
        _dbIrs_valuesChanged = 0;
        adc_selectChannel(ADC_AVCC, _dbIrs_sensorIndex);
        adc_trigger10(_dbIrs_measured);
    }

    return _dbIrs_retriggerTime_ms;
}

// Starts the continuous measurement of distances
void dbIrs_startContinuousMeasurements(uint8_t sensors, uint16_t time_ms, void (*changedCallback)())
{
    if (!_dbIrs_initialized)
    {
        uart0_msg("dbIrs_startContinuousMeasurements: dbIrs_init missing\n");
        return;
    }

    if (!(sensors & (DB_IRS_SENSOR_FRONT | DB_IRS_SENSOR_BACK | DB_IRS_SENSOR_LEFT | DB_IRS_SENSOR_RIGHT))) // if none of the infrared sensors is selected
    {
        uart0_msg("dbIrs_startContinuousMeasurements: no sensor selected\n");
        return;
    }

    _dbIrs_sensors = (sensors >> 4);
    _dbIrs_readyCallback = NULL;
    _dbIrs_changedCallback = changedCallback;
    _dbIrs_retriggerTime_ms = time_ms;

    dbIrs_stopContinuousMeasurements();
    _dbIrs_handle = tb_register(_dbIrs_continuousMeasurement, time_ms);
    if (!_dbIrs_handle)
    {
        uart0_msg("dbIrs_startContinuousMeasurements: could not register tb-callback\n");
        return;
    }
};

uint8_t dbIrs_doesContinuouslyMeasure()
{
    return _dbIrs_handle != 0;
}