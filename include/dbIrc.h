// ----------------------------------------------------------------------------
/// @file         dbIrc.h
/// @addtogroup   DBIRC_LIB   DB-IRC Library (libdbirc.a, dbirc.h)
/// @{
/// @brief        The DB-IRC library provides functions to communicate with the
///               DiscBot's infrared sender and the infrared receiver.
///               The infrared receiver makes use of the timebase-library, which
///               must therefore be initialized (tb_init) before the receiver
///               can be initialized. The infrared transmission follows the
///               NEC infrared protocol, thus each command comprises 32 bits.
/// @author       Dietmar Scheiblhofer
// ----------------------------------------------------------------------------

#ifndef DBIRC_H_
#define DBIRC_H_

#include <avr/io.h>

#ifdef __cplusplus
extern "C" {
  #endif

  // ----------------------------------------------------------------------------
  /// @brief        Initializes the infrared system. The DiscBot's infrared
  ///               receiver makes use of the timebase, which must be initialized
  ///               prior to this function.
  // ----------------------------------------------------------------------------
  void dbIrc_init();

  // ----------------------------------------------------------------------------
  /// @brief        Checks if the DB-IRC library got initialized
  /// @retval       0               the library was not initialized
  /// @retval       1               the library was initialized
  // ----------------------------------------------------------------------------
  uint8_t dbIrc_isInitialized();

  // ----------------------------------------------------------------------------
  /// @brief        Registers a function to be called whenever a command has been
  ///               received be the infrared receiver.
  /// @param[in]    callback        function to be called, when a command has been
  ///                               received
  // ----------------------------------------------------------------------------
  void dbIrc_registerReceptionCallback(void(*callback)(uint32_t command));

  // ----------------------------------------------------------------------------
  /// @brief        Sends a command over the infrared sender. The function is
  ///               blocking and thus suppressing all interrupts for approx. 10ms
  /// @param[in]    command         the command to be sent
  // ----------------------------------------------------------------------------
  void dbIrc_send(uint32_t command);

  // ----------------------------------------------------------------------------
  /// @brief        Behaves as if the command were received over the infrared
  ///               interface
  /// @param[in]    command         the received command
  /// @retval       0               no reception callback registered
  /// @retval       1               a reception callback was registered
  // ----------------------------------------------------------------------------
  uint8_t dbIrc_received(uint32_t command);

  uint8_t dbIrc_callbackRegistered();
  // ----------------------------------------------------------------------------
  /// @brief        Checks if a callback function is registered to process commands
  /// @retval       0               no reception callback registered
  /// @retval       1               a reception callback was registered
  // ----------------------------------------------------------------------------

  #ifdef __cplusplus
};
#endif

#endif /* DBIRC_H_ */
/// @}