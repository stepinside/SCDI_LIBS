// ----------------------------------------------------------------------------
/// @file         dbIrs.h
/// @addtogroup   DBIRS_LIB   DB-IRS Library (libdbirs.a, dbirs.h)
/// @{
/// @brief        The DB-IRS library provides functions to measure the discbot's distance to other objects
///               with the help of infrared sensors
/// @author       Dietmar Scheiblhofer
// ----------------------------------------------------------------------------

#ifndef DB_IRS_H_
#define DB_IRS_H_

#include <avr/io.h>

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

// ----------------------------------------------------------------------------
/// @brief			  used to select one or more sensors
#define DB_IRS_SENSOR_FRONT (1 << 7) ///< front sensor
#define DB_IRS_SENSOR_BACK (1 << 5)  ///< back sensor
#define DB_IRS_SENSOR_LEFT (1 << 4)  ///< left sensor
#define DB_IRS_SENSOR_RIGHT (1 << 6) ///< right sensor

#ifdef __cplusplus
extern "C"
{
#endif

    // ----------------------------------------------------------------------------
    /// @brief        Initializes the sensors and the DB-IRS library
    // ----------------------------------------------------------------------------
    void dbIrs_init();

    // ----------------------------------------------------------------------------
    /// @brief        Checks if the DB-IRS library got initialized
    /// @retval       0               the library was not initialized
    /// @retval       1               the library was initialized
    // ----------------------------------------------------------------------------
    uint8_t dbIrs_isInitialized();

    // ----------------------------------------------------------------------------
    /// @brief        Triggers a single distance measurement.
    /// @param[in]    sensors         sensor(s) to do the measurement. The values DB_IRS_SENSOR_FRONT,
    ///                               DB_IRS_SENSOR_LEFT, DB_IRS_SENSOR_RIGHT, DB_IRS_SENSOR_BACK can
    ///                               be or-ed to measure the distance by several sensors at once.
    /// @param[in]    readyCallback   function to be called, once the distance(s) has/have been
    ///                               measured
    // ----------------------------------------------------------------------------
    void dbIrs_triggerSingleMeasurement(uint8_t sensors, void (*readyCallback)(const struct DbDistances *pDistances));

    // ----------------------------------------------------------------------------
    /// @brief        Starts to continuously measure the distances.
    /// @param[in]    sensors         sensor(s) to do the measurements. The values DB_IRS_SENSOR_FRONT,
    ///                               DB_IRS_SENSOR_LEFT, DB_IRS_SENSOR_RIGHT, DB_IRS_SENSOR_BACK can
    ///                               be or-ed to measure the distance by several sensors at once.
    /// @param[in]    time_ms         measurement intervall; time_ms must be a multiple of the timebase's
    ///                               basetime.
    /// @param[in]    changedCallback function to be called, when at least one distance changed since
    ///                               the last measurement; changedCallback will be called at least once.
    // ----------------------------------------------------------------------------
    void dbIrs_startContinuousMeasurements(uint8_t sensors,
                                           uint16_t time_ms,
                                           void (*changedCallback)(const struct DbDistances *pDistances));

    // ----------------------------------------------------------------------------
    /// @brief        Stops to continuously measure the distances.
    // ----------------------------------------------------------------------------
    void dbIrs_stopContinuousMeasurements();

    // ----------------------------------------------------------------------------
    /// @brief        Checks if the system continuously measures the distances.
    // ----------------------------------------------------------------------------
    uint8_t dbIrs_doesContinuouslyMeasure();

#ifdef __cplusplus
};
#endif

#endif /* DB_IRS_H_ */
       /// @}