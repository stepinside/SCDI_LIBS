// ----------------------------------------------------------------------------
/// @file         dbLed.h
/// @addtogroup   DBLED_LIB   DB-LED Library (libdbled.a, dbled.h)
/// @{
/// @brief        The DB-LED library provides functions to control the color of the RGB LEDs.
/// @author       Dietmar Scheiblhofer
// ----------------------------------------------------------------------------

#ifndef DB_LED_H_
#define DB_LED_H_

#include <avr/io.h>

// ----------------------------------------------------------------------------
/// @brief			  Ids of the differend LEDs; the values are self explaining
enum DB_LED_ID
{
    DB_LED_FRONT_RIGHT_OUT = 0,
    DB_LED_FRONT_RIGHT_IN = 1,
    DB_LED_FRONT_LEFT_IN = 2,
    DB_LED_FRONT_LEFT_OUT = 3,

    DB_LED_BUTTON_BLUE = 4,
    DB_LED_BUTTON_GREEN = 5,
    DB_LED_BUTTON_RED = 6,

    DB_LED_BACK_LEFT_OUT = 7,
    DB_LED_BACK_LEFT_IN = 8,
    DB_LED_BACK_RIGHT_IN = 9,
    DB_LED_BACK_RIGHT_OUT = 10
};

#define DB_LED_FIRST 0
#define DB_LED_LAST 10

#ifdef __cplusplus
extern "C"
{
#endif

    // ----------------------------------------------------------------------------
    /// @brief        Initializes the DB-LED library
    // ----------------------------------------------------------------------------
    void dbLed_init();

    // ----------------------------------------------------------------------------
    /// @brief        Checks if the DB-LED library got initialized
    /// @retval       0               the library was not initialized
    /// @retval       1               the library was initialized
    // ----------------------------------------------------------------------------
    uint8_t dbLed_isInitialized();

    // ----------------------------------------------------------------------------
    /// @brief        Sets the color of a certain RGB-LED. dbLed_setColor calls
    ///               dbLed_prepareColor and afterwards dbLed_refresh, which lasts approx. 5-6ms.
    ///               In case several LEDs shall be updated at once, it is therefore
    ///               faster to prepare the LEDs by dbLed_prepareColor an refreshing them
    ///               afterwards all at once.
    /// @param[in]    ledId           the id of the LED
    /// @param[in]    r               the color's red component
    /// @param[in]    g               the color's green component
    /// @param[in]    b               the color's blue component
    // ----------------------------------------------------------------------------
    void dbLed_setColor(enum DB_LED_ID ledId, uint8_t r, uint8_t g, uint8_t b);

    // ----------------------------------------------------------------------------
    /// @brief        Prepares the color of a certain RGB-LED. The color will be updated,
    ///               when dbLed_refresh is called. When several colors shall be changed
    ///               it's better to prepare the colors and do a refresh, when all colors
    ///               have been prepared.
    /// @param[in]    ledId           the id of the LED
    /// @param[in]    r               the color's red component
    /// @param[in]    g               the color's green component
    /// @param[in]    b               the color's blue component
    // ----------------------------------------------------------------------------
    void dbLed_prepareColor(enum DB_LED_ID ledId, uint8_t r, uint8_t g, uint8_t b);

    // ----------------------------------------------------------------------------
    /// @brief        Refreshes the colors of all RGB-LEDs. Refreshing the LEDs lasts
    ///               approx. 5-6 ms.
    // ----------------------------------------------------------------------------
    void dbLed_refresh();

#ifdef __cplusplus
};
#endif

#endif /* DB_LED_H_ */
       /// @}