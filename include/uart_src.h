static uint8_t uartx_initialized = 0;

void uartx_init(uint32_t baudrate, enum UartMode mode, enum UartParity parity)
{
    uartx_initialized = 1;

    // set all bits according the default setting; i.e. 2 stop bits, 8 data bits, async mode
    UCSRxA = 0; // clears all bits and U2X0 for single transmission speed

    UCSRxB = 0;                               // clears all bits and UCSZ02 for 8 data bits
    UCSRxC = ((1 << USBS0) |                  // 2 stop bits
              (1 << UCSZ01) | (1 << UCSZ00)); // 8 data bits
                                              // clears all other bits for async mode

    // set the baudrate
    if (baudrate > 57600)
    {
        baudrate /= 2;
        UCSRxA |= (1 << U2X0);
    }
    UBRRx = round(F_CPU / 16 / (double)baudrate) - 1; // set baudrate

    // set the mode
    switch (mode)
    {
    case UART_M_RECEIVE: // reception only
    {
        UCSRxB |= (1 << RXEN0);
        UCSRxB &= ~(1 << TXEN0);
        break;
    }
    case UART_M_TRANSMIT: // transmission only
    {
        UCSRxB |= (1 << TXEN0);
        UCSRxB &= ~(1 << RXEN0);
        break;
    }
    case UART_M_TRANSCEIVE: // reception and transmission
    {
        UCSRxB |= (1 << RXEN0);
        UCSRxB |= (1 << TXEN0);
        break;
    }
    }

    // set the parity configuration
    switch (parity)
    {
    case UART_P_NONE:
    {
        UCSRxC &= ~((1 << UPM01) | (1 << UPM00)); // parity none
        break;
    }
    case UART_P_EVEN:
    {
        UCSRxC |= (1 << UPM01); // parity even
        UCSRxC &= ~(1 << UPM00);
        break;
    }
    case UART_P_ODD: // parity odd
    {
        UCSRxC |= (1 << UPM01);
        UCSRxC |= (1 << UPM00);
        break;
    }
    }
}

void uartx_putc(char c)
{
    while (!(UCSRxA & (1 << UDRE0)))
        ;     // wait until the UDRE0 bit has been set. This
              // indicates that the transmission buffer is ready
              // for another character to send
    UDRx = c; // put the character into the transmission buffer
}

void uartx_puts(char *pString)
{
    while (*pString != '\0') // as long as there are characters left
                             // the end of the string is indicated by '\0'
    {
        while (!(UCSRxA & (1 << UDRE0)))
            ;            // wait until the transmission buffer is ready
                         // for the next character
        UDRx = *pString; // copy the character into the transmission buffer
        pString++;       // continue with the next character
    }
}

char uartx_getc()
{
    while (!(UCSRxA & (1 << RXC0)))
        ;        // wait until a character has been received
                 // A character is available when the RXC0 flag
                 // has been set.
    return UDRx; // return the received character
}

uint8_t uartx_getc_nb(char *pData)
{
    if (!(UCSRxA & (1 << RXC0)))
        return 0;

    *pData = UDRx;
    return 1;
}

void uartx_msg(char *pString)
{
    if (!uartx_initialized)
    {
        uartx_init(115200, UART_M_TRANSCEIVE, UART_P_NONE);
    }
    uartx_puts(pString);
}
