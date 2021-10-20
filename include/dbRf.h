// ----------------------------------------------------------------------------
/// @file         dbRf.h
/// @addtogroup   DBRF_LIB   DB-RF Library (libdbrf.a, dbrf.h)
/// @{
/// @brief        The DB-RF library provides functions to measure the discbot's distance to other objects
///               with the help of infrared or ultrasonic sensors
/// @author       Dietmar Scheiblhofer
// ----------------------------------------------------------------------------

#ifndef DB_RF_H_
#define DB_RF_H_

#include <avr/io.h>
#include <dbUss.h>
#include <dbIrs.h>

#ifndef DB_DISTANCES
#define DB_DISTANCES

struct DbDistances
{
    uint8_t front_cm;
    uint8_t back_cm;
    uint8_t left_cm;
    uint8_t right_cm;
};
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    // ----------------------------------------------------------------------------
    /// @brief        Initializes the sensors, the DB-IRS and DB-USS library
    // ----------------------------------------------------------------------------
    void dbRf_init();

    // ----------------------------------------------------------------------------
    /// @brief        Checks if the DB-RF library got initialized
    /// @retval       0               the library was not initialized
    /// @retval       1               the library was initialized
    // ----------------------------------------------------------------------------
    uint8_t dbRf_isInitialized();

    // ----------------------------------------------------------------------------
    /// @brief        Triggers a single distance measurement.
    /// @param[in]    sensors         sensor(s) to do the measurement. The values DB_IRS_SENSOR_FRONT,
    ///                               DB_IRS_SENSOR_LEFT, DB_IRS_SENSOR_RIGHT, DB_IRS_SENSOR_BACK,
    ///                               DB_USS_SENSOR_FRONT, DB_USS_SENSOR_LEFT, DB_USS_SENSOR_RIGHT, DB_USS_SENSOR_BACK
    ///                               can be or-ed to measure the distance by several sensors at once.
    /// @param[in]    readyCallback   function to be called, once the distance(s) has/have been
    ///                               measured
    // ----------------------------------------------------------------------------
    void dbRf_triggerSingleMeasurement(uint8_t sensors, void (*readyCallback)(const struct DbDistances *pDistances));

    // ----------------------------------------------------------------------------
    /// @brief        Starts to continuously measure the distances.
    /// @param[in]    sensors         sensor(s) to do the measurements. The values DB_USS_SENSOR_FRONT,
    ///                               DB_USS_SENSOR_LEFT, DB_USS_SENSOR_RIGHT, DB_USS_SENSOR_BACK,
    ///                               DB_IRS_SENSOR_FRONT, DB_IRS_SENSOR_LEFT, DB_IRS_SENSOR_RIGHT, DB_IRS_SENSOR_BACK
    ///                               can be or-ed to measure the distance by several sensors at once.
    ///                               Selected sensors must be connected to the discbot. Otherwise the
    ///                               measurement will fail.
    /// @param[in]    time_ms         measurement intervall; time_ms must be a multiple of the timebase's
    ///                               basetime.
    /// @param[in]    readyCallback   function to be called, whenever the distance(s) has/have been
    ///                               measured; when readyCallback is NULL, then no function will be called
    ///                               when the measurement is done
    /// @param[in]    changedCallback function to be called, when at least one distance changed since
    ///                               the last measurement; when changedCallcack is NULL, the no function
    ///                               will be called, when the distances changed
    // ----------------------------------------------------------------------------
    void dbRf_startContinuousMeasurements(uint8_t sensors,
                                          uint16_t time_ms,
                                          void (*changedCallback)(const struct DbDistances *pDistances));

    // ----------------------------------------------------------------------------
    /// @brief        Stops to continuously measure the distances.
    // ----------------------------------------------------------------------------
    void dbRf_stopContinuousMeasurements();

    // ----------------------------------------------------------------------------
    /// @brief        Checks if the system continuously measures the distances.
    // ----------------------------------------------------------------------------
    uint8_t dbRf_doesContinuouslyMeasure();

#ifdef __cplusplus
};
#endif

#endif /* DB_RF_H_ */