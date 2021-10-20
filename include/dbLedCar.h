// ----------------------------------------------------------------------------
/// @file         dbLedCar.h
/// @addtogroup   DBLEDCAR_LIB   DB-LEDCAR Library (libdbledcar.a, dbledcar.h)
/// @{
/// @brief        The DB-LEDCAR library provides functions to control the color of the RGB LEDs as
///               if the robot were a car.
/// @author       Dietmar Scheiblhofer
// ----------------------------------------------------------------------------

#ifndef DB_LED_CAR_H_
#define DB_LED_CAR_H_

// ----------------------------------------------------------------------------
/// @brief			  used to describe the DiscBot's light state
#define DB_LED_CAR_LIGHT 0x01
#define DB_LED_CAR_HEADLIGHT 0x02
#define DB_LED_CAR_BRAKELIGHT 0x04
#define DB_LED_CAR_BACKLIGHT 0x08

// ----------------------------------------------------------------------------
/// @brief			  used to control the car's indicator
enum DbLedCarIndicator
{
    DB_LED_CAR_INDICATOR_OFF = 0,   ///< the indicator is off
    DB_LED_CAR_INDICATOR_LEFT = 1,  ///< indicate a left turn
    DB_LED_CAR_INDICATOR_RIGHT = 2, ///< indicate a right turn
    DB_LED_CAR_INDICATOR_BOTH = 3   ///< indicate an alarm
};

#ifdef __cplusplus
extern "C"
{
#endif

    // ----------------------------------------------------------------------------
    /// @brief        Initializes the DB-LEDCAR library
    // ----------------------------------------------------------------------------
    void dbLedCar_init();

    // ----------------------------------------------------------------------------
    /// @brief        Checks if the DB-LEDCAR library got initialized
    /// @retval       0               the library was not initialized
    /// @retval       1               the library was initialized
    // ----------------------------------------------------------------------------
    uint8_t dbLedCar_isInitialized();

    // ----------------------------------------------------------------------------
    /// @brief        updates the LEDs according to the current state
    // ----------------------------------------------------------------------------
    void dbLedCar_update();

    // ----------------------------------------------------------------------------
    /// @brief        Gets the current state of the DiscBot's lights
    /// @return       the current state. See DB_LED_CAR_LIGHT, DB_LED_CAR_HEADLIGHT,
    ///               DB_LED_CAR_BRAKELIGHT, DB_LED_CAR_BACKLIGHT combinations are
    ///               possible
    // ----------------------------------------------------------------------------
    uint8_t dbLedCar_getState();

    // ----------------------------------------------------------------------------
    /// @brief        Gets the current state of the DiscBot's indicator
    /// @return       the current state of the indicators. See DB_LED_CAR_INDICATOR_OFF,
    ///               DB_LED_CAR_INDICATOR_LEFT, DB_LED_CAR_INDICATOR_RIGHT,
    ///               DB_LED_CAR_INDICATOR_BOTH
    // ----------------------------------------------------------------------------
    enum DbLedCarIndicator dbLedCar_getIndicator();

    // ----------------------------------------------------------------------------
    /// @brief        Turns the normal light on/off
    /// @param[in]    on      0 -> turn the light off
    ///                       otherwise -> turn the light on
    // ----------------------------------------------------------------------------
    void dbLedCar_light(uint8_t on);

    // ----------------------------------------------------------------------------
    /// @brief        Turns the head light on/off
    /// @param[in]    on      0 -> turn the light off
    ///                       otherwise -> turn the light on
    // ----------------------------------------------------------------------------
    void dbLedCar_headlight(uint8_t on);

    // ----------------------------------------------------------------------------
    /// @brief        Turns the brake light on/off
    /// @param[in]    on      0 -> turn the light off
    ///                       otherwise -> turn the light on
    // ----------------------------------------------------------------------------
    void dbLedCar_brakelight(uint8_t on);

    // ----------------------------------------------------------------------------
    /// @brief        Turns the back light on/off
    /// @param[in]    on      0 -> turn the light off
    ///                       otherwise -> turn the light on
    // ----------------------------------------------------------------------------
    void dbLedCar_backlight(uint8_t on);

    // ----------------------------------------------------------------------------
    /// @brief        Turns the indicators on/off
    /// @param[in]    indicator      DB_LED_CAR_INDICATOR_OFF | DB_LED_CAR_INDICATOR_LEFT |
    ///                              DB_LED_CAR_INDICATOR_RIGHT | DB_LED_CAR_INDICATOR_BOTH
    // ----------------------------------------------------------------------------
    void dbLedCar_indicator(enum DbLedCarIndicator indicator);

#ifdef __cplusplus
};
#endif

#endif /* DB_LED_H_ */
/// @}
