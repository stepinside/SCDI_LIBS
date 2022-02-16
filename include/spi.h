// ----------------------------------------------------------------------------
/// @file         spi.h
/// @addtogroup   SPI_LIB   SPI Library (libspi.a, spi.h)
/// @{
/// @brief        The SPI library configures the SPI module as master and allows to communicate with SPI slaves
/// @details      For this following signals need to be connected with the SPI slave
///               master.
///               @li SPI_MOSI (PortB.2, Arduino2560: Pin 51) > SPI_MOSI
///               @li SPI_MISO (PortB.3, Arduino2560: Pin 50) > SPI_MISO
///               @li SPI_SCK  (PortB.1, Arduino2560: Pin 52) > SPI_SCK
///               @li any pin  (e.g. PortB.0, Arduino2560: Pin 53) > SPI_SS   (Slave select)
/// @author       Dietmar Scheiblhofer
// ----------------------------------------------------------------------------

#ifndef SPI_H
#define SPI_H

#include <avr/io.h>

#define SPI_DDR		DDRB
#define SPI_PORT  PORTB
#define SPI_PIN   PINB
#define SPI_MOSI	PB2
#define SPI_MISO	PB3
#define SPI_SCK		PB1
#define SPI_SS    PB0

#ifdef __cplusplus
extern "C" {
  #endif

  // ----------------------------------------------------------------------------
  /// @brief        Configures the SPI module for master operation; sets the prescaler to 16
  // ----------------------------------------------------------------------------
  void spi_init();

  // ----------------------------------------------------------------------------
  /// @brief        Checks if the SPI module has been initialized.
  /// @retval       1               yes
  /// @retval       0               no
  // ----------------------------------------------------------------------------
  uint8_t spi_isInitialized();

  // ----------------------------------------------------------------------------
  /// @brief        Exchanges data between the SPI module an a connected SPI slave.
  ///               The slave's select line (SPI_SS) must have been set to 0
  ///               before the transmission starts and must be set back to 1, once
  ///               the transmission is over.
  /// @param[in]    data    the data to be transmitted to the spi slave
  /// @return       the data received from the slave
  // ----------------------------------------------------------------------------
  uint8_t spi_transceive(uint8_t data);

  #ifdef __cplusplus
};
#endif

#endif

/// @}
