// ----------------------------------------------------------------------------
/// @file         dbUss.h
/// @addtogroup   DBUSS_LIB   DB-USS Library (libdbuss.a, dbuss.h)
/// @{
/// @brief        The DB-USS library provides functions to measure the discbot's distance to other objects
///               with the help of ultrasonic sensors
/// @author       Dietmar Scheiblhofer
// ----------------------------------------------------------------------------

#ifndef DB_USS_H_
#define DB_USS_H_

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
#define DB_USS_SENSOR_FRONT (1 << 3) ///< front sensor
#define DB_USS_SENSOR_LEFT (1 << 0)  ///< left sensor
#define DB_USS_SENSOR_RIGHT (1 << 2) ///< right sensor
#define DB_USS_SENSOR_BACK (1 << 1)  ///< back sensor

#ifdef __cplusplus
extern "C"
{
#endif

    // ----------------------------------------------------------------------------
    /// @brief        Initializes the sensors and the DB-USS library
    // ----------------------------------------------------------------------------
    void dbUss_init();

    // ----------------------------------------------------------------------------
    /// @brief        Checks if the DB-USS library got initialized
    /// @retval       0               the library was not initialized
    /// @retval       1               the library was initialized
    // ----------------------------------------------------------------------------
    uint8_t dbUss_isInitialized();

    // sensor must be connected!
    // ----------------------------------------------------------------------------
    /// @brief        Triggers a single distance measurement.
    /// @param[in]    sensors         sensor(s) to do the measurement. The values DB_USS_SENSOR_FRONT,
    ///                               DB_USS_SENSOR_LEFT, DB_USS_SENSOR_RIGHT, DB_USS_SENSOR_BACK can
    ///                               be or-ed to measure the distance by several sensors at once.
    ///                               Selected sensors must be connected to the discbot. Otherwise the
    ///                               measurement will fail.
    /// @param[in]    readyCallback   function to be called, once the distance(s) has/have been
    ///                               measured
    // ----------------------------------------------------------------------------
    void dbUss_triggerSingleMeasurement(uint8_t sensors, void (*readyCallback)(const struct DbDistances *pDistances));

    // ----------------------------------------------------------------------------
    /// @brief        Starts to continuously measure the distances.
    /// @param[in]    sensors         sensor(s) to do the measurements. The values DB_USS_SENSOR_FRONT,
    ///                               DB_USS_SENSOR_LEFT, DB_USS_SENSOR_RIGHT, DB_USS_SENSOR_BACK can
    ///                               be or-ed to measure the distance by several sensors at once.
    ///                               Selected sensors must be connected to the discbot. Otherwise the
    ///                               measurement will fail.
    /// @param[in]    time_ms         measurement intervall; time_ms must be a multiple of the timebase's
    ///                               basetime.
    /// @param[in]    changedCallback function to be called, when at least one distance changed since
    ///                               the last measurement; changedCallback will be called at least once.
    // ----------------------------------------------------------------------------
    void dbUss_startContinuousMeasurements(uint8_t sensors,
                                           uint16_t time_ms,
                                           void (*changedCallback)(const struct DbDistances *pDistances));

    // ----------------------------------------------------------------------------
    /// @brief        Stops to continuously measure the distances.
    // ----------------------------------------------------------------------------
    void dbUss_stopContinuousMeasurements();

    // ----------------------------------------------------------------------------
    /// @brief        Checks if the system continuously measures the distances.
    // ----------------------------------------------------------------------------
    uint8_t dbUss_doesContinuouslyMeasure();

#ifdef __cplusplus
};
#endif

#endif /* DB_USS_H_ */
       /// @}