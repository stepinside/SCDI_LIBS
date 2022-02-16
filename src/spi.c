#include "spi.h"
#include <uart.h>

static uint8_t spi_initialized = 0;

void spi_init()
{
  if (spi_initialized)
  {
    uart0_msg("spi_init: already initialized\n");
    return;
  }

  spi_initialized = 1;
  SPI_DDR |= (1<<SPI_MOSI)|(1<<SPI_SCK)|(1<<SPI_SS);
  SPI_DDR &= ~(1 << SPI_MISO);
  SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);                //prescaler 16
}
// ----------------------------------------------------------------------------

uint8_t spi_isInitialized()
{
  return spi_initialized;
}

uint8_t spi_transceive(uint8_t data)
{
  if (!spi_initialized)
  {
    uart0_msg("spi_transceive: spi_init missing\n");
    return 0;
  }
  SPDR = data;
  while(!(SPSR & (1<<SPIF)));
  return SPDR;
}
