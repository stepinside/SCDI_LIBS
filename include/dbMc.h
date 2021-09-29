// ----------------------------------------------------------------------------
/// @file         dbMc.h
/// @addtogroup   dbMc_LIB   DB-MC Library (libdbmc.a, dbmc.h)
/// @{
/// @brief        The DB-MC library provides functions to move the DiscBot
/// @author       Dietmar Scheiblhofer
// ----------------------------------------------------------------------------

#ifndef DB_MC_H_
#define DB_MC_H_

#include <avr/io.h>

#ifdef __cplusplus
extern "C" {
  #endif

  // ----------------------------------------------------------------------------
  /// @brief        Initializes the DB-MC library
  ///               If the green button is pressed, when this function is called,
  ///               then the function dbMc_calibrate is automatically called and
  ///               the motion control system gets calibrated. This is necessary
  ///               due to production tolerances of the motors. In case a PS2
  ///               controller is available then dbMc_manualCalibrate is called
  ///               instead of dbMc_calibrate
  // ----------------------------------------------------------------------------
  void dbMc_init();

  // ----------------------------------------------------------------------------
  /// @brief        Checks if the DB-MC library got initialized
  /// @retval       0               the library was not initialized
  /// @retval       1               the library was initialized
  // ----------------------------------------------------------------------------
  uint8_t dbMc_isInitialized();

  // ----------------------------------------------------------------------------
  /// @brief        Sets the speed of the DiscBot in centimeters per second for the left and right wheel
  /// @param[in]    speed_cmps      -200=max. speed backwards; 0=stop; +200=max. speed forwards
  // ----------------------------------------------------------------------------
  void dbMc_setSpeed(int16_t speed_cmps);

  // ----------------------------------------------------------------------------
  /// @brief        Sets the speed of the DiscBot's left wheel in centimeters per second
  /// @param[in]    speed_cmps      -200=max. speed backwards; 0=stop; +200=max. speed forwards
  // ----------------------------------------------------------------------------
  void dbMc_setSpeedLeft(int16_t speed_cmps);

  // ----------------------------------------------------------------------------
  /// @brief        Sets the speed of the DiscBot's right wheel in centimeters per second
  /// @param[in]    speed_cmps           -200=max. speed backwards; 0=stop; +200=max. speed forwards
  // ----------------------------------------------------------------------------
  void dbMc_setSpeedRight(int16_t speed_cmps);

  // ----------------------------------------------------------------------------
  /// @brief        Gets the speed of the DiscBot in centimeters per second as it was set by dbMc_setSpeed
  /// @return       the speed of the DiscBot
  // ----------------------------------------------------------------------------
  int16_t dbMc_getSpeed();

  // ----------------------------------------------------------------------------
  /// @brief        Gets the speed of the DiscBot's left wheel in centimeters per second
  /// @return       the speed of the DiscBot's left wheel
  // ----------------------------------------------------------------------------
  int16_t dbMc_getSpeedLeft();

  // ----------------------------------------------------------------------------
  /// @brief        Gets the speed of the DiscBot's right wheel in centimeters per second
  /// @return       the speed of the DiscBot's right wheel
  // ----------------------------------------------------------------------------
  int16_t dbMc_getSpeedRight();

  // ----------------------------------------------------------------------------
  /// @brief        Registers a function to be called when the speed of the DiscBot changed
  /// @param[im]    changedCallback   function to be called, when the speed of the DiscBot
  ///                                 is about to be changed. If the callback function returns
  ///                                 1, the speed will be updated, otherwise not.
  // ----------------------------------------------------------------------------
  void dbMc_registerSpeedChangedCallback(uint8_t (*changedCallback)(int16_t oldSpeed_cmps, int16_t newSpeed_cmps));

  // ----------------------------------------------------------------------------
  /// @brief        Registers a function to be called when the direction of the DiscBot changed
  /// @param[im]    changedCallback   function to be called, when the direction of the DiscBot
  ///                                 is about to be changed. If the callback function returns
  ///                                 1, the direction will be updated, otherwise not.
  // ----------------------------------------------------------------------------
  void dbMc_registerDirectionChangedCallback(uint8_t (*changedCallback)(int8_t oldDirection, int8_t newDirection));

  // ----------------------------------------------------------------------------
  /// @brief        Brakes both wheels by turning them shortly in the opposite direction
  /// @param[in]    doneCallback    function to be called, when the wheels stopped turning
  // ----------------------------------------------------------------------------
  void dbMc_brake(void (*doneCallback)());

  // ----------------------------------------------------------------------------
  /// @brief        Brakes the left wheel by turning it shortly in the opposite direction
  /// @param[in]    doneCallback    function to be called, when the wheel stopped turning
  // ----------------------------------------------------------------------------
  void dbMc_brakeLeft(void (*doneCallback)());

  // ----------------------------------------------------------------------------
  /// @brief        Brakes the right wheel by turning it shortly in the opposite direction
  /// @param[in]    doneCallback    function to be called, when the wheel stopped turning
  // ----------------------------------------------------------------------------
  void dbMc_brakeRight(void (*doneCallback)());

  // ----------------------------------------------------------------------------
  /// @brief        Sets the direction of the DiscBot
  /// @param[in]    direction       -100=max. right turn; 0=straight ahead; +100=max. left turn
  // ----------------------------------------------------------------------------
  void dbMc_setDirection(int8_t direction);

  // ----------------------------------------------------------------------------
  /// @brief        Gets the direction of the DiscBot
  /// @return       the direction of the DiscBot
  // ----------------------------------------------------------------------------
  int8_t dbMc_getDirection();

  // ----------------------------------------------------------------------------
  /// @brief        Sets the speed and direction of the DiscBot
  /// @param[in]    speed_cmps      -200=max. speed backwards; 0=stop; +200=max. speed forwards
  /// @param[in]    direction       -100=max. right turn; 0=straight ahead; +100=max. left turn
  // ----------------------------------------------------------------------------
  void dbMc_setSpeedAndDirection(int16_t speed_cmps, int8_t direction);

  // ----------------------------------------------------------------------------
  /// @brief        Rotates the DiscBot
  /// @param[in]    angle           the angle by which the DiscBot shall be rotated.
  ///                               angle > 0 ... rotates clockwise, angle < 0 ... counter clockwise
  /// @param[in]    speed           speed to rotate the DiscBot; 0 <= speed <= 100
  /// @param[in]    doneCallback    function to be called, when the rotation is done; if this
  ///                               value is NULL, no function will be called
  // ----------------------------------------------------------------------------
  void dbMc_rotate(int16_t angle, uint8_t speed, void (*doneCallback)());

  // ----------------------------------------------------------------------------
  /// @brief        Moves the DiscBot
  /// @param[in]    distance_mm     the distance in mm by which the DiscBot shall be moved.
  ///                               If the DiscBot drives along a curve, the distance of the
  ///                               outer wheel is considered.
  /// @param[in]    speed           speed to move the DiscBot; -100 <= speed <= 100; negative
  ///                               values move the DiscBot backwards
  /// @param[in]    doneCallback    function to be called, when the motion is done; if this
  ///                               value is NULL, no function will be called
  // ----------------------------------------------------------------------------
  void dbMc_move(uint16_t distance_mm, int16_t speed, void (*doneCallback)());

  // ----------------------------------------------------------------------------
  /// @brief        Sets the circumference of the DiscBot's wheels. The value gets stored
  ///               in the EEPROM and will thus be permanent.
  /// @param[in]    circumference_mm    the circumference in mm of the DiscBot's wheels.
  ///                               The value must be between 180 and 230.
  /// @retval       0               an invalid circumference was given
  /// @retval       1               the circumference was successfully set
  // ----------------------------------------------------------------------------
  uint8_t dbMc_setWheelCircumference(uint8_t circumference_mm);

  #ifdef __cplusplus
};
#endif


#endif /* DB_MC_H_ */
/// @}
