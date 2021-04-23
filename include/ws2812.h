// ----------------------------------------------------------------------------
/// @file         neopixel.h
/// @addtogroup   WS2812_LIB   WS2812 Library (libws2812.a, ws2812.h)
/// @{
/// @brief        The WS2812 library provides functions to realize a 2-dimensional
///               display with WS2812 cascadable RGB-LEDs
/// @author       Dietmar Scheiblhofer
// ----------------------------------------------------------------------------

#ifndef WS2812_H_
#define WS2812_H_

#include <avr/io.h>

// ----------------------------------------------------------------------------
/// @brief        contains all data required for the display
typedef struct
{
    uint8_t *buffer; ///< pointer to the display's data; for each pixel a byte for red, green and blue is stored
    uint8_t width;   ///< width of the display
    uint8_t height;  ///< height of the display
} Ws2812Display;
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/// @brief        Creates a 2-dimensional display by reserving the respective memory
/// @param[in]    width       the number of pixels in x direction
/// @param[in]    height      the number of pixels in y direction
/// @retVal       NULL        too less memory to create the display
/// @retVal       !NULL       the display
// ----------------------------------------------------------------------------
Ws2812Display *ws2812_create(uint8_t width, uint8_t height);

// ----------------------------------------------------------------------------
/// @brief        Deletes the given display and frees the allocated memory
/// @param[in]    display   the display to be deleted
// ----------------------------------------------------------------------------
void ws2812_delete(Ws2812Display *display);

// ----------------------------------------------------------------------------
/// @brief        Sets the pixel of the given display to the respective RGB value.
///               The pixel is just prepared and the RGB-LEDs get updated by the next call of ws2812_refresh.
/// @param[in]    display           the display to be used
/// @param[in]    x                 the pixel's x-position
/// @param[in]    y                 the pixel's y-position
/// @param[in]    red, green, blue  the color of the pixel
// ----------------------------------------------------------------------------
void ws2812_setPixel(Ws2812Display *display, uint8_t x, uint8_t y, uint8_t red, uint8_t green, uint8_t blue);

// ----------------------------------------------------------------------------
/// @brief        Refreshes the display
/// @param[in]    display     the display to be refreshed
/// @param[in]    pPort       the port the RGB-LEDs are connected to; e.g. &PORTA
/// @param[in]    portPin     the pin, the data pin of the neopixel is connected to; e.g. 0
// ----------------------------------------------------------------------------
void ws2812_refresh(Ws2812Display *display, volatile uint8_t *pPort, uint8_t portPin);

#endif /* WS2812_H_ */