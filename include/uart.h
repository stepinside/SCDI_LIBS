// ----------------------------------------------------------------------------
/// @file         uart_basic.h
/// @addtogroup   UART_BASIC_LIB   UART_BASIC Library (libuart_basic.a, uart_basic.h)
/// @{
/// @brief        The uart_basic library provides functions to configure UART0-UART3 and functions to send and receive data.
///               The library does NOT support interrupt-driven data exchange.
/// @author       Dietmar Scheiblhofer
// ----------------------------------------------------------------------------

#ifndef UART_BASIC_H_
#define UART_BASIC_H_

/// @cond HIDDEN_SYMBOLS
#ifndef F_CPU
#define F_CPU 16000000
#endif
/// @endcond

#include <avr/io.h>

// ----------------------------------------------------------------------------
/// @brief			  used to set the UART's parity mode
enum UartParity
{
    UART_P_NONE, ///< no parity
    UART_P_EVEN, ///< parity even
    UART_P_ODD   ///< parity odd
};
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/// @brief			  used to configure the UART for reception and/or transmission
enum UartMode
{
    UART_M_RECEIVE,   ///< reception only
    UART_M_TRANSMIT,  ///< transmission only
    UART_M_TRANSCEIVE ///< reception and transmission
};
// ----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif

// ----------------------------------------------------------------------------
/// @brief        Configures UARTx for data reception and/or data transmission in asynchronous mode with 8 data and 2 stop bits.
/// @param[in]    baudrate        the baudrate
/// @param[in]    mode            the mode
/// @param[in]    parity          the parity configuration
// ----------------------------------------------------------------------------
#if 0
  void uartn_init(uint32_t baudrate, enum UartMode mode, enum UartParity parity);     // used for doxygen documentation; function does not exist
#endif
    /// @cond HIDDEN_SYMBOLS
    void uart0_init(uint32_t baudrate, enum UartMode mode, enum UartParity parity);
    void uart1_init(uint32_t baudrate, enum UartMode mode, enum UartParity parity);
    void uart2_init(uint32_t baudrate, enum UartMode mode, enum UartParity parity);
    void uart3_init(uint32_t baudrate, enum UartMode mode, enum UartParity parity);
/// @endcond

// ----------------------------------------------------------------------------
/// @brief        Waits until the transmission buffer of UARTx is empty and sends the given character.
/// @param[in]    c               the character to send
// ----------------------------------------------------------------------------
#if 0
  void uartn_putc(char c);
#endif
    /// @cond HIDDEN_SYMBOLS
    void uart0_putc(char c);
    void uart1_putc(char c);
    void uart2_putc(char c);
    void uart3_putc(char c);
/// @endcond

// ----------------------------------------------------------------------------
/// @brief        Sends the given string over UARTx. uartn_puts is a blocking
///               function which means that it waits until all characters of
///               the string have been sent.
/// @param[in]    pString         the string to send
// ----------------------------------------------------------------------------
#if 0
  void uartn_puts(char *pString);
#endif
    /// @cond HIDDEN_SYMBOLS
    void uart0_puts(char *pString);
    void uart1_puts(char *pString);
    void uart2_puts(char *pString);
    void uart3_puts(char *pString);
/// @endcond

// ----------------------------------------------------------------------------
/// @brief        Waits until a character has been received by UARTx and returns the character.
/// @return       the received character
// ----------------------------------------------------------------------------
#if 0
  char uartn_getc();
#endif
    /// @cond HIDDEN_SYMBOLS
    char uart0_getc();
    char uart1_getc();
    char uart2_getc();
    char uart3_getc();
/// @endcond

// ----------------------------------------------------------------------------
/// @brief        Checks if data are available and returns them if so
/// @param[out]   pData   the data received
/// @retval       1   data available
/// @retval       0   otherwise
// ----------------------------------------------------------------------------
#if 0
  uint8_t uartn_getc_nb(char *pData);
#endif
    /// @cond HIDDEN_SYMBOLS
    uint8_t uart0_getc_nb(char *pData);
    uint8_t uart1_getc_nb(char *pData);
    uint8_t uart2_getc_nb(char *pData);
    uint8_t uart3_getc_nb(char *pData);
/// @endcond

// ----------------------------------------------------------------------------
/// @brief        Sends the given string over UART0. If the UART has not been
///               initialized yet, it will be set to 115200 Baud and no parity
/// @param[in]    pString   the string to be sent
// ----------------------------------------------------------------------------
#if 0
  void uartn_msg(char *pString);
#endif
    /// @cond HIDDEN_SYMBOLS
    void uart0_msg(char *pString);
    void uart1_msg(char *pString);
    void uart2_msg(char *pString);
    void uart3_msg(char *pString);
    /// @endcond

#ifdef __cplusplus
};
#endif

#endif /* UART_BASIC_H_ */
/// @}
