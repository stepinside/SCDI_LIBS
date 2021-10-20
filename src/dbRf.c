#include "dbRf.h"

#include <stdlib.h>

#include <tb.h>
#include <uart.h>

#include <dbUss.h>
#include <dbIrs.h>

static struct DbDistances _dbRf_distances;
static uint8_t _dbRf_sensors;
static void (*_dbRf_readyCallback)(const struct DbDistances *pDistances) = NULL;
static void (*_dbRf_changedCallback)(const struct DbDistances *pDistances) = NULL;
static uint8_t _dbRf_readyCnt;

static uint8_t _dbRf_initialized = 0;

void dbRf_init()
{
    if (_dbRf_initialized)
        return;
    _dbRf_initialized = 1;

    dbUss_init();
    dbIrs_init();
}

uint8_t dbRf_isInitialized()
{
    return _dbRf_initialized;
}

void _dbRf_readyIrs(const struct DbDistances *pDistances)
{
    if (_dbRf_sensors & DB_IRS_SENSOR_BACK)
        _dbRf_distances.back_cm = pDistances->back_cm;
    if (_dbRf_sensors & DB_IRS_SENSOR_FRONT)
        _dbRf_distances.front_cm = pDistances->front_cm;
    if (_dbRf_sensors & DB_IRS_SENSOR_LEFT)
        _dbRf_distances.left_cm = pDistances->left_cm;
    if (_dbRf_sensors & DB_IRS_SENSOR_RIGHT)
        _dbRf_distances.right_cm = pDistances->right_cm;

    _dbRf_readyCnt--;
    if (!_dbRf_readyCnt)
    {
        (*_dbRf_readyCallback)(&_dbRf_distances);
    }
}

void _dbRf_changedIrs(const struct DbDistances *pDistances)
{
    if (_dbRf_sensors & DB_IRS_SENSOR_BACK)
        _dbRf_distances.back_cm = pDistances->back_cm;
    if (_dbRf_sensors & DB_IRS_SENSOR_FRONT)
        _dbRf_distances.front_cm = pDistances->front_cm;
    if (_dbRf_sensors & DB_IRS_SENSOR_LEFT)
        _dbRf_distances.left_cm = pDistances->left_cm;
    if (_dbRf_sensors & DB_IRS_SENSOR_RIGHT)
        _dbRf_distances.right_cm = pDistances->right_cm;

    (*_dbRf_changedCallback)(&_dbRf_distances);
}

void _dbRf_readyUss(const struct DbDistances *pDistances)
{
    _dbRf_readyCnt--;

    if (_dbRf_sensors & DB_USS_SENSOR_BACK)
        _dbRf_distances.back_cm = pDistances->back_cm;
    if (_dbRf_sensors & DB_USS_SENSOR_FRONT)
        _dbRf_distances.front_cm = pDistances->front_cm;
    if (_dbRf_sensors & DB_USS_SENSOR_LEFT)
        _dbRf_distances.left_cm = pDistances->left_cm;
    if (_dbRf_sensors & DB_USS_SENSOR_RIGHT)
        _dbRf_distances.right_cm = pDistances->right_cm;

    if (!_dbRf_readyCnt)
    {
        (*_dbRf_readyCallback)(&_dbRf_distances);
    }
}

void _dbRf_changedUss(const struct DbDistances *pDistances)
{
    if (_dbRf_sensors & DB_USS_SENSOR_BACK)
        _dbRf_distances.back_cm = pDistances->back_cm;
    if (_dbRf_sensors & DB_USS_SENSOR_FRONT)
        _dbRf_distances.front_cm = pDistances->front_cm;
    if (_dbRf_sensors & DB_USS_SENSOR_LEFT)
        _dbRf_distances.left_cm = pDistances->left_cm;
    if (_dbRf_sensors & DB_USS_SENSOR_RIGHT)
        _dbRf_distances.right_cm = pDistances->right_cm;

    (*_dbRf_changedCallback)(&_dbRf_distances);
}

void dbRf_triggerSingleMeasurement(uint8_t sensors, void (*readyCallback)(const struct DbDistances *pDistances))
{
    if (!_dbRf_initialized)
    {
        uart0_msg("dbRf_triggerSingleMeasurement: dbRf_init missing\n");
        return;
    }
    _dbRf_distances.front_cm = _dbRf_distances.back_cm = _dbRf_distances.left_cm = _dbRf_distances.right_cm = 255;
    _dbRf_sensors = sensors;

    _dbRf_readyCallback = readyCallback;
    _dbRf_changedCallback = NULL;
    _dbRf_readyCnt = 0;

    if (sensors & 0x0F)
    {
        _dbRf_readyCnt++;
        dbUss_triggerSingleMeasurement(sensors & 0x0F, _dbRf_readyUss);
    }
    if (sensors & 0xF0)
    {
        _dbRf_readyCnt++;
        dbIrs_triggerSingleMeasurement(sensors & 0xF0, _dbRf_readyIrs);
    }
}

void dbRf_startContinuousMeasurements(uint8_t sensors, uint16_t time_ms, void (*changedCallback)(const struct DbDistances *pDistances))
{
    if (!_dbRf_initialized)
    {
        uart0_msg("dbRf_startContinuousMeasurements: dbRf_init missing\n");
        return;
    }

    _dbRf_distances.front_cm = _dbRf_distances.back_cm = _dbRf_distances.left_cm = _dbRf_distances.right_cm = 255;
    _dbRf_sensors = sensors;
    _dbRf_readyCallback = NULL;
    _dbRf_changedCallback = changedCallback;

    if (sensors & 0x0F)
    {
        dbUss_startContinuousMeasurements(sensors & 0x0F, time_ms, _dbRf_changedUss);
    }
    if (sensors & 0xF0)
    {
        dbIrs_startContinuousMeasurements(sensors & 0xF0, time_ms, _dbRf_changedIrs);
    }
}

void dbRf_stopContinuousMeasurements()
{
    if (!_dbRf_initialized)
    {
        uart0_msg("dbRf_stopContinuousMeasurements: dbRf_init missing\n");
        return;
    }

    dbUss_stopContinuousMeasurements();
    dbIrs_stopContinuousMeasurements();
}

uint8_t dbRf_doesContinuouslyMeasure()
{
    return ((dbIrs_doesContinuouslyMeasure() || dbUss_doesContinuouslyMeasure()) != 0);
}
