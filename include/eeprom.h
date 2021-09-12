// ----------------------------------------------------------------------------
/// @file         eeprom.h
/// @addtogroup   EEPROM_LIB   EEProm Library (libeeprom.a, eeprom.h)
/// @{
/// @brief        The EEProm library provides a set of functions to read data from and write data into the eeprom
/// @author       Dietmar Scheiblhofer
// ----------------------------------------------------------------------------

#ifndef EEPROM_H_
#define EEPROM_H_

#include <avr/io.h>

#ifdef __cplusplus
extern "C"
{
#endif

    // -----------------------------------------------------------------
    /// @brief        Writes a single byte into the eeprom.
    /// @param[in]    address     the address to write the data to
    /// @param[in]    data        the data to be written
    // -----------------------------------------------------------------
    void eeprom_write(uint16_t address, uint8_t data);
    // -----------------------------------------------------------------

    // -----------------------------------------------------------------
    /// @brief        Writes two bytes into the eeprom.
    /// @param[in]    address     the address to write the data to
    /// @param[in]    data        the data to be written
    // -----------------------------------------------------------------
    void eeprom_write16(uint16_t address, uint16_t data);
    // -----------------------------------------------------------------

    // -----------------------------------------------------------------
    /// @brief        Reads a single byte from the eeprom.
    /// @param[in]    address     the address to read the data from
    /// @return       the byte read
    // -----------------------------------------------------------------
    uint8_t eeprom_read(uint16_t address);
    // -----------------------------------------------------------------

    // -----------------------------------------------------------------
    /// @brief        Reads two bytes from the eeprom.
    /// @param[in]    address     the address to read the data from
    /// @return       the byte read
    // -----------------------------------------------------------------
    uint16_t eeprom_read16(uint16_t address);
    // -----------------------------------------------------------------

#ifdef __cplusplus
};
#endif

#endif /* EEPROM_H_ */

/// @}
