#include <avr/io.h>
#include <string.h>

#include <tb.h>
#include <sr04.h>
#include <uart.h>

#include "dbUss.h"

static struct DbDistances   _dbUss_distances;
static volatile uint8_t     _dbUss_sensors;
static volatile uint8_t     _dbUss_handle = 0;
static uint8_t              _dbUss_initialized = 0;

static void (*_dbUss_readyCallback)(const struct DbDistances* pDistances);
static void (*_dbUss_changedCallback)(const struct DbDistances* pDistances);
static uint16_t             _dbUss_retriggerTime_ms = 0;

static void _dbUss_measured(uint16_t distances_mm[8]);
static uint16_t _dbUss_continuousMeasurement();


void dbUss_init()
{
  if (_dbUss_initialized)   return;

  _dbUss_initialized = 1;

  DDRH |= (1 << 5);       // trigger pulse
  DDRK &= ~0x0f;
  PORTK &= ~0x0f;         // deactivate pullups
}

uint8_t dbUss_isInitialized()
{
  return _dbUss_initialized;
}


// Stops the continuous measurement of distances
void dbUss_stopContinuousMeasurements()
{
  if (!_dbUss_initialized)
  {
    uart0_msg("dbUss_stopContinuousMeasurements: dbUss_init missing\n");
    return;
  }

  if (_dbUss_handle != 0)
  {
    tb_unregister(_dbUss_handle);
    _dbUss_handle = 0;
  }
}


void _dbUss_measured(uint16_t distances_mm[8])
{
  uint8_t i;
  uint8_t changed = 0;

  for (i=0; i<4; i++)
  {
    if (_dbUss_sensors & (1 << i))
    {
      uint8_t distance_cm = distances_mm[i] / 10;
      uint8_t* pActDistance = NULL;

      switch(i)
      {
        case 0: pActDistance = &(_dbUss_distances.left_cm);    break;
        case 1: pActDistance = &(_dbUss_distances.back_cm);    break;
        case 2: pActDistance = &(_dbUss_distances.right_cm);   break;
        case 3: pActDistance = &(_dbUss_distances.front_cm);   break;
      }

      // the sr04 ultrasonic sensors sometimes deliver incorrect distances
      // mostly below 5cm -> thus, ignore distances below 5cm

      if ((pActDistance != NULL) && (*pActDistance != distance_cm) && (distance_cm > 5))
      {
        *pActDistance = distance_cm;
        changed = 1;
      }
    }
  }

  if (_dbUss_readyCallback)
  {
    _dbUss_readyCallback(&_dbUss_distances);
  }
  if (changed && _dbUss_changedCallback)
  {
    _dbUss_changedCallback(&_dbUss_distances);
  }
}

// stops continuous measurements
void dbUss_triggerSingleMeasurement(uint8_t sensors, void (*readyCallback)(const struct DbDistances* pDistances))
{
  if (!_dbUss_initialized)
  {
    uart0_msg("dbUss_triggerSingleMeasurement: dbUss_init missing\n");
    return;
  }

  dbUss_stopContinuousMeasurements();

  _dbUss_readyCallback = readyCallback;
  _dbUss_changedCallback = NULL;

  _dbUss_sensors = sensors;

  sr04_getDistance(sensors, &PORTH, 5, _dbUss_measured);
};


uint16_t _dbUss_continuousMeasurement()
{
  sr04_getDistance(_dbUss_sensors, &PORTH, 5, _dbUss_measured);
  return _dbUss_retriggerTime_ms;
}

// Starts the continuous measurement of distances
void dbUss_startContinuousMeasurements(uint8_t sensors, uint16_t time_ms, void (*changedCallback)())
{
  if (!_dbUss_initialized)
  {
    uart0_msg("dbUss_startContinuousMeasurements: dbUss_init missing\n");
    return;
  }

  if (!(sensors & (DB_USS_SENSOR_FRONT | DB_USS_SENSOR_BACK | DB_USS_SENSOR_LEFT | DB_USS_SENSOR_RIGHT)))      // if none of the ultrasonic sensors is selected
  {
    uart0_msg("dbUss_startContinuousMeasurements: no sensor selected\n");
    return;
  }

  _dbUss_sensors = sensors;
  _dbUss_readyCallback = NULL;
  _dbUss_changedCallback = changedCallback;
  _dbUss_retriggerTime_ms = time_ms;

  dbUss_stopContinuousMeasurements();
  _dbUss_handle = tb_register(_dbUss_continuousMeasurement, time_ms);
  if (!_dbUss_handle)
  {
    uart0_msg("dbUss_startContinuousMeasurements: could not register tb-callback\n");
    return;
  }
};

  // ----------------------------------------------------------------------------
  /// @brief        Checks if the system continuously measures the distances.
  // ----------------------------------------------------------------------------
uint8_t dbUss_doesContinuouslyMeasure()
{
  return (_dbUss_handle != 0);
}
