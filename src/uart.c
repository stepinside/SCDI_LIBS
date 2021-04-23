#include "uart.h"
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// ----------------------------------------------------------------------------
// generate code for UART0
// ----------------------------------------------------------------------------
#define uartx_init uart0_init
#define uartx_putc uart0_putc
#define uartx_puts uart0_puts
#define uartx_getc uart0_getc
#define uartx_getc_nb uart0_getc_nb
#define uartx_msg uart0_msg
#define UCSRxA UCSR0A
#define UCSRxB UCSR0B
#define UCSRxC UCSR0C
#define UBRRx UBRR0
#define UDRx UDR0
#define uartx_initialized uart0_initialized
#include "uart_src.h"
#undef uartx_init
#undef uartx_putc
#undef uartx_puts
#undef uartx_getc
#undef uartx_msg
#undef uartx_getc_nb
#undef UCSRxA
#undef UCSRxB
#undef UCSRxC
#undef UBRRx
#undef UDRx
#undef uartx_initialized

// ----------------------------------------------------------------------------
// generate code for UART1
// ----------------------------------------------------------------------------
#define uartx_init uart1_init
#define uartx_putc uart1_putc
#define uartx_puts uart1_puts
#define uartx_getc uart1_getc
#define uartx_getc_nb uart1_getc_nb
#define uartx_msg uart1_msg
#define UCSRxA UCSR1A
#define UCSRxB UCSR1B
#define UCSRxC UCSR1C
#define UBRRx UBRR1
#define UDRx UDR1
#define uartx_initialized uart1_initialized
#include "uart_src.h"
#undef uartx_init
#undef uartx_putc
#undef uartx_puts
#undef uartx_getc
#undef uartx_getc_nb
#undef uartx_msg
#undef UCSRxA
#undef UCSRxB
#undef UCSRxC
#undef UBRRx
#undef UDRx
#undef uartx_initialized

// ----------------------------------------------------------------------------
// generate code for UART2
// ----------------------------------------------------------------------------
#define uartx_init uart2_init
#define uartx_putc uart2_putc
#define uartx_puts uart2_puts
#define uartx_getc uart2_getc
#define uartx_getc_nb uart2_getc_nb
#define uartx_msg uart2_msg
#define UCSRxA UCSR2A
#define UCSRxB UCSR2B
#define UCSRxC UCSR2C
#define UBRRx UBRR2
#define UDRx UDR2
#define uartx_initialized uart2_initialized
#include "uart_src.h"
#undef uartx_init
#undef uartx_putc
#undef uartx_puts
#undef uartx_getc
#undef uartx_getc_nb
#undef uartx_msg
#undef UCSRxA
#undef UCSRxB
#undef UCSRxC
#undef UBRRx
#undef UDRx
#undef uartx_initialized

// ----------------------------------------------------------------------------
// generate code for UART3
// ----------------------------------------------------------------------------
#define uartx_init uart3_init
#define uartx_putc uart3_putc
#define uartx_puts uart3_puts
#define uartx_getc uart3_getc
#define uartx_getc_nb uart3_getc_nb
#define uartx_msg uart3_msg
#define UCSRxA UCSR3A
#define UCSRxB UCSR3B
#define UCSRxC UCSR3C
#define UBRRx UBRR3
#define UDRx UDR3
#define uartx_initialized uart3_initialized
#include "uart_src.h"
#undef uartx_init
#undef uartx_putc
#undef uartx_puts
#undef uartx_getc
#undef uartx_getc_nb
#undef uartx_msg
#undef UCSRxA
#undef UCSRxB
#undef UCSRxC
#undef UBRRx
#undef UDRx
#undef uartx_initialized
