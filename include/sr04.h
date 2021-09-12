// ----------------------------------------------------------------------------
/// @file         sr04.h
/// @addtogroup   SR04_LIB   SR04 Library (libsr04.a, sr04.h)
/// @{
/// @brief        The SR04 library provides a function to measure distances with the help of
///               the ultra sonic sensors HC-SR04.
/// @details      The echo signal of the sensors must be connected to the pins of port K.
///               The trigger signal of all sensors are linked together and can be connected
///               to any digital output pin of the controller. The library makes use of
///               timer2, which is therefore not available for other operations.
/// @author       Dietmar Scheiblhofer
// ----------------------------------------------------------------------------
#ifndef SR04_H_
#define SR04_H_

#include <avr/io.h>

#define SR04_SENSOR0 (1 << 0)
#define SR04_SENSOR1 (1 << 1)
#define SR04_SENSOR2 (1 << 2)
#define SR04_SENSOR3 (1 << 3)
#define SR04_SENSOR4 (1 << 4)
#define SR04_SENSOR5 (1 << 5)
#define SR04_SENSOR6 (1 << 6)
#define SR04_SENSOR7 (1 << 7)

#ifdef __cplusplus
extern "C"
{
#endif

    // ----------------------------------------------------------------------------
    /// @brief        performs distance measurements with the help of the ultrasonic sensors (HC-SR04).
    /// @details      The function can measure the distance of several sensors at once; which of the eight possible sensors shall perform
    ///               the measurement is defined by the function parameter 'sensors'. Use the defines SR04_SENSORx for this purpose.
    ///               The echo signals of the sensors need to be connected to one of the pins of port K. SR04_SENSOR0 for example must be
    //                connected to pinK.0; SR04_SENSOR1 must be connected to pinK.1 and so forth.
    /// @param[in]    sensors     defines which sensors shall perform a measurement; use the defines SR04_SENSORx; if several sensors
    ///                           shall do the measurement at once, then just or the defines like: SR04_SENSOR0 | SR04_SENSOR4.
    /// @param[in]    pTriggerPort      defines the port of the trigger pin, that triggers the sensors
    /// @param[in]    triggerPinNo      defines the pin that triggers the measurement. The specified pin needs to be connected to
    ///                                 the trigger pin of all sensors.
    /// @param[in]    callback          defines the function that is called once the measurement of all selected sensors in done. The
    ///                                 callback function gets as parameter an array of the measured distances. The distance of sensor
    ///                                 SR04_SENSOR0 is stored in distances_mm[0], the distance of sensor SR04_SENSOR1 in distances_mm[1], etc.
    ///                                 All distances are given in millimeters.
    // ----------------------------------------------------------------------------
    void sr04_getDistance(uint8_t sensors, volatile uint8_t *pTriggerPort, uint8_t triggerPinNo, void (*callback)(uint16_t distances_mm[8]));

#ifdef __cplusplus
};
#endif

#endif /* SR04_H_ */

/// @}