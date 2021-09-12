/*
 * EEprom_Library.c
 *
 * Created: 03.10.2013 08:52:53
 *  Author: Didi
 */

#include <avr/io.h>

void eeprom_write(uint16_t address, uint8_t data)
{
    /* Wait for completion of previous write */
    while (EECR & (1 << EEPE))
        ;
    /* Set up address and Data Registers */
    EEAR = address;
    EEDR = data;
    /* Write logical one to EEMPE */
    EECR |= (1 << EEMPE);
    /* Start eeprom write by setting EEPE */
    EECR |= (1 << EEPE);
}

void eeprom_write16(uint16_t address, uint16_t data)
{
    eeprom_write(address, data >> 8);
    eeprom_write(address + 1, data & 0xFF);
}

uint8_t eeprom_read(uint16_t address)
{
    /* Wait for completion of previous write */
    while (EECR & (1 << EEPE))
        ;
    /* Set up address register */
    EEAR = address;
    /* Start eeprom read by writing EERE */
    EECR |= (1 << EERE);
    /* Return data from Data Register */
    return EEDR;
}

uint16_t eeprom_read16(uint16_t address)
{
    return (((uint16_t)eeprom_read(address)) << 8) + eeprom_read(address + 1);
}