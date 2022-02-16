#ifndef MFRC522_H
#define MFRC522_H

#include "mfrc522_cmd.h"
#include "mfrc522_reg.h"
#include <avr/io.h>

// ----------------------------------------------------------------------------
// This library provides functions to interact with the MFRC522 - RFID module.
// the communication between the µC and the RFID-module is done over SPI.
// thus following connections must be established between the µC and the module
// µC                                               RFID-module
// ----------------------------------------------------------------------------
// SPI_MOSI (PinB.2, Arduino2560: Pin 51)           MOSI
// SPI_MISO (PinB.3, Arduino2560: Pin 50)           MISO
// SPI_SCK  (PinB.1, Arduino2560: Pin 52)           SCK
// open                                             IRQ
// 3.3V                                             3.3V
// RESET                                            RST
// GND                                              GND
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// name:          mfrc522_init
// description:   initializes the mfrc522 chip for RFID communication
//                since the communication with the RFID chip is done over SPI,
//                the SPI interface get automatically initialized for master
//                mode. the mfrc522 library assumes that the RFID module's
//                SPI_SS (slave select) signal is connected to PinB.0
// parameters:    pSpiPort -> the port controlling the spi's slave select
//                            signal of the RFID reader (e.g. &PORTB
//                spiPortPin -> the pin number of the spi's slave select signal
// ----------------------------------------------------------------------------
void mfrc522_init(volatile uint8_t *pSpiPort, uint8_t spiPortPin);

// ----------------------------------------------------------------------------
// name:          mfrc522_reset
// description:   reset the RFID chip. When the RFID-chip got reseted, it needs
//                to be initialized with mfrc522_init again.
// ----------------------------------------------------------------------------
void mfrc522_reset();

// ----------------------------------------------------------------------------
// name:          mfrc522_detectCard
// description:   checks if there is a card close to the RFID-reader
// return value:  1 -> a card is close to the reader
//                0 -> no card is close to the reader
// ----------------------------------------------------------------------------
uint8_t	mfrc522_detectCard();

// ----------------------------------------------------------------------------
// name:          mfrc522_write
// description:   writes data into the given register
// parameters:    reg -> the register to write data to
//                data -> the data to be written
// ----------------------------------------------------------------------------
void mfrc522_write(uint8_t reg, uint8_t data);

// ----------------------------------------------------------------------------
// name:          mfrc522_read
// description:   reads data from the given register
// parameters:    reg -> the register to read data from
// return value:  the data read from the register
// ----------------------------------------------------------------------------
uint8_t mfrc522_read(uint8_t reg);

// ----------------------------------------------------------------------------
// name:          mfrc522_getVersion
// description:   gets the version of the RFID-chip
// return value:  0 -> no RFID-chip detected
//                otherwise -> the chip version
// ----------------------------------------------------------------------------
uint8_t mfrc522_getVersion();

// ----------------------------------------------------------------------------
// name:          mfrc522_getUID
// description:   gets the UID of the RFID-card read be the RFID-reader
//                before the UID can be retrieved, a card needs to be detected
//                by mfrc522_detectCard
// parameters:    UID -> the UID of the card
// return value:  0 -> no card was found
//                1 -> otherwise
// ----------------------------------------------------------------------------
uint8_t mfrc522_getUID(uint32_t* pUid);

#endif
