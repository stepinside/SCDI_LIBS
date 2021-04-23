
// ----------------------------------------------------------------------------
/// @file         timebase.h
/// @addtogroup   TIMEBASE_LIB   TIMEBASE Library (libtimebase.a, timebase.h)
/// @{
/// @brief        The TIMEBASE library allows to call functions at certain times.
/// @details      First, the timebase needs to be initialized with @ref tbInit. Afterwards functions can
///               be registered that get called by the timebase when their time has come.
///               The timebase occupies <b>timer 1</b>, which therefore cannot be used for other purposes.
/// @author       Dietmar Scheiblhofer
// ----------------------------------------------------------------------------

#ifndef TIMEBASE_H_
#define TIMEBASE_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

// ----------------------------------------------------------------------------
/// @brief			  used to set the timebase
enum TB_BaseTime
{
  TB_10MS = 10,   ///< 10 milliseconds
  TB_20MS = 20,   ///< 20 milliseconds
  TB_50MS = 50,   ///< 50 milliseconds
  TB_100MS = 100, ///< 100 milliseconds
  TB_200MS = 200, ///< 200 milliseconds
  TB_500MS = 500, ///< 500 milliseconds
  TB_1S = 1000    ///< 1 second
};

#ifdef __cplusplus
extern "C"
{
#endif

  // ----------------------------------------------------------------------------
  /// @brief        Initializes the timebase, by setting up timer 1 and reserving memory for
  ///               the given number of function callbacks.
  /// @param[in]    baseTime_ms     the time, when a registered function shall be called, is a multiple of
  ///                               the given baseTime_ms (in milliseconds). Setting baseTime_ms to a
  ///                               small value, allows to define more accurate times, but on the other hand
  ///                               consumes more processing power of the CPU. Typical values of baseTime_ms
  ///                               are TB_50MS and TB_100MS (see @ref enum TB_TimeBase).
  /// @param[in]    maxCallbacks    the maximum number of functions that can be registered
  /// @retval       1               ok
  /// @retval       0               something went wrong; e.g. the requested memory could not be allocated
  // ----------------------------------------------------------------------------
  uint8_t tb_init(enum TB_BaseTime baseTime_ms, uint8_t maxCallbacks);

  // ----------------------------------------------------------------------------
  /// @brief        Checks if the timebase got initialized.
  /// @retval       1               yes
  /// @retval       0               no
  // ----------------------------------------------------------------------------
  uint8_t tb_isInitialized();

  // ----------------------------------------------------------------------------
  /// @brief        Returns the time in milliseconds since the timebase got initialized.
  /// @return       the time in milliseconds
  // ----------------------------------------------------------------------------
  uint32_t tb_getTime_ms();

  // ----------------------------------------------------------------------------
  /// @brief        Returns the timeBase's baseTime in milliseconds.
  /// @return       the baseTime in milliseconds
  // ----------------------------------------------------------------------------
  uint16_t tb_getBaseTime_ms();

  // ----------------------------------------------------------------------------
  /// @brief        Registers a function to be called in time_ms milliseconds; this starts a timeout for the function which, once reached,
  ///               will call the function. The timeout can be manipulated by @ref tbResetTimeout, @ref tbStopTimeout, @ref tbStartTimeout.
  /// @param[in]    callback        the function to be called. A called function can determine by its return value,
  ///                               whether it shall be called again. If the return value = 0, the function will not
  ///                               be called again, otherwise it will be called in the returned number of miliseconds
  ///                               The return value must be a multiple a the timebase's basetime.
  /// @param[in]    time_ms         the function shall be called in time_ms milliseconds. time_ms must be a multiple
  ///                               of the baseTime_ms set at @ref tbInit.
  /// @retval       0               the function could not be registered
  /// @retval       >0              the function's handle in the timebase
  // ----------------------------------------------------------------------------
  uint8_t tb_register(uint16_t (*callback)(), uint16_t time_ms);

  // ----------------------------------------------------------------------------
  /// @brief        Unregisters a function from the timebase.
  /// @param[in]    handle          the function's handle obtained when the function got registered (@ref tbRegister).
  /// @retval       1               ok, the function was unregistered
  /// @retval       0               the function could not be unregistered
  // ----------------------------------------------------------------------------
  uint8_t tb_unregister(uint8_t handle);

  // ----------------------------------------------------------------------------
  /// @brief        Resets the function's timeout.
  /// @param[in]    handle          the function's handle obtained when the function got registered (@ref tbRegister).
  /// @retval       1               ok, the timeout was reset
  /// @retval       0               the timeout could not be reset
  // ----------------------------------------------------------------------------
  uint8_t tb_resetTimeout(uint8_t handle);

  // ----------------------------------------------------------------------------
  /// @brief        Stops the function's timeout but does not reset it.
  /// @param[in]    handle          the function's handle obtained when the function got registered (@ref tbRegister).
  /// @retval       1               ok, the timeout was stopped
  /// @retval       0               the timeout could not be stopped
  // ----------------------------------------------------------------------------
  uint8_t tb_stopTimeout(uint8_t handle);

  // ----------------------------------------------------------------------------
  /// @brief        Starts the function's timeout, when it got stopped before.
  /// @param[in]    handle          the function's handle obtained when the function got registered (@ref tbRegister).
  /// @retval       1               ok, the timeout was started again
  /// @retval       0               the timeout could not be started
  // ----------------------------------------------------------------------------
  uint8_t tb_startTimeout(uint8_t handle);

#ifdef __cplusplus
};
#endif

#endif /* TIMEBASE_H_ */

/// @}