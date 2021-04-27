// ----------------------------------------------------------------------------
/// @file         dbBtn.h
/// @addtogroup   DBBTN_LIB   DB-BTN Library (libdbbtn.a, dbbtn.h)
/// @{
/// @brief        The DB-BTN library provides functions to control the DiscBot's buttons. The
///               buttons are debounce by software, so that only those changes get reported that
///               took longer than 70ms
/// @author       Dietmar Scheiblhofer
// ----------------------------------------------------------------------------

#ifndef DB_BTN_H_
#define DB_BTN_H_

enum DbBtn
{
    DB_BTN_RED = (1 << 4),
    DB_BTN_GREEN = (1 << 5),
    DB_BTN_BLUE = (1 << 6)
};

#ifdef __cplusplus
extern "C"
{
#endif

    // ----------------------------------------------------------------------------
    /// @brief        Initializes the DB-BTN library
    // ----------------------------------------------------------------------------
    void dbBtn_init();

    // ----------------------------------------------------------------------------
    /// @brief        Checks if dbBtn got initialized.
    /// @retval       1               yes
    /// @retval       0               no
    // ----------------------------------------------------------------------------
    uint8_t dbBtn_isInitialized();

    // ----------------------------------------------------------------------------
    /// @brief        Checks if the selected button is currenty pressed
    /// @param        the button to be tested: either DB_BTN_RED, DB_BTN_GREEN or DB_BTN_BLUE
    /// @retval       1               the button is currently pressed
    /// @retval       0               the button is currently not pressed
    // ----------------------------------------------------------------------------
    uint8_t dbBtn_isPressed(enum DbBtn button);

    // ----------------------------------------------------------------------------
    /// @brief        Registers a callback function to be called when the red button
    ///               got pressed or released.
    /// @param[in]    pCallback       the function to be called when the state of the button
    ///                               changed. The function's state parameter =1, when the
    ///                               button is currently pressed otherwise its 0.
    // ----------------------------------------------------------------------------
    void dbBtn_registerRedCallback(void (*pCallback)(uint8_t state));

    // ----------------------------------------------------------------------------
    /// @brief        Registers a callback function to be called when the green button
    ///               got pressed or released.
    /// @param[in]    pCallback       the function to be called when the state of the button
    ///                               changed. The function's state parameter =1, when the
    ///                               button is currently pressed otherwise its 0.
    // ----------------------------------------------------------------------------
    void dbBtn_registerGreenCallback(void (*pCallback)(uint8_t state));

    // ----------------------------------------------------------------------------
    /// @brief        Registers a callback function to be called when the blue button
    ///               got pressed or released.
    /// @param[in]    pCallback       the function to be called when the state of the button
    ///                               changed. The function's state parameter =1, when the
    ///                               button is currently pressed otherwise its 0.
    // ----------------------------------------------------------------------------
    void dbBtn_registerBlueCallback(void (*pCallback)(uint8_t state));

#ifdef __cplusplus
};
#endif

#endif /* DB_BTN_H_ */
       /// @}