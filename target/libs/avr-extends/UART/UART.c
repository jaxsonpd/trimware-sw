/** 
 * @file UART_Test.c
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2024-03-30
 * @brief A file to test the uart interface on the avr microcontrollers
 * 
 * 
 * @cite http://www.rjhcoding.com/avrc-uart.php
 */

#include <stdint.h>
#include <stdio.h>

#include <avr/io.h>
#include <util/delay.h>

#include "../avr_extends/UART.h"
#include "../avr_extends_utils.h"

static int putc_stdio(char c, FILE *stream);
static int getc_stdio(FILE *stream);

// Stream setup see https://www.nongnu.org/avr-libc/user-manual/group__avr__stdio.html
// Stdin example to implement https://stackoverflow.com/questions/4023895/how-do-i-read-a-string-entered-by-the-user-in-c
// static FILE g_stdout = FDEV_SETUP_STREAM(putc_stdio, NULL, _FDEV_SETUP_WRITE);
// static FILE g_stdin = FDEV_SETUP_STREAM(NULL, getc_stdio, _FDEV_SETUP_READ);


void UART_init (uint32_t baud) {
    // set the baudrate
    unsigned long ubrr = (((F_CPU) + 8UL * (baud)) / (16UL * (baud))) - 1UL;

    UBRR0L = (uint8_t)(ubrr & 0xFF);
    UBRR0H = (uint8_t)(ubrr >> 8);

    // enable the uart pins
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);    

    // Set operation 8 data 1 stop 0 parity
    UCSR0C = (3 << UCSZ00);
}


void UART_init_stdio (uint32_t baud) {
    UART_init(baud);

    // Assign in/out functions to stdio
    FILE *UNUSED(stream) = fdevopen(putc_stdio, getc_stdio);
}

/** 
 * @brief Put a char to the serial monitor for use in stdio calls
 * @param c the character to send
 * @param stream a pointer to the file stream object
 * 
 * @return 0 if succesfull
 */
static int putc_stdio(char c, FILE *UNUSED(stream)) {
    // wait for transmit buffer to be empty
    while(!(UCSR0A & (1 << UDRE0)));

    // load data into transmit register
    UDR0 = c;

    return 0;
}


/** 
 * @brief read a char from the serial monitor for use by stdio
 * @param stream a pointer to the file stream
 * 
 * @return the char read
 */
static int getc_stdio(FILE *UNUSED(stream)) {
    // Wait until buffer is full
    while(!(UCSR0A & (1 << RXC0)));

    // return the data
    return UDR0;
}



void UART_putc (unsigned char data) {
    // wait for transmit buffer to be empty
    while(!(UCSR0A & (1 << UDRE0)));

    // load data into transmit register
    UDR0 = data;
}


void UART_puts(char* s) {
    // Transmit until null is reached
    while (*s > 0) {
        UART_putc (*s++);
    }
}

void UART_puthex8(uint8_t val) {
    // extract hex values
    uint8_t upperNibble = (val & 0xF0) >> 4;
    uint8_t lowerNibble = val & 0x0F;

    // convert to ascii
    upperNibble += upperNibble > 9 ? 'A' - 10 : '0';
    lowerNibble += lowerNibble > 9 ? 'A' - 10 : '0';

    UART_putc(upperNibble);
    UART_putc(lowerNibble);
}



unsigned char UART_getc(void) {
    // Wait until buffer is full
    while(!(UCSR0A & (1 << RXC0)));

    // return the data
    return UDR0;
}


uint16_t UART_get_line(char* p_buffer, uint16_t bufferLength) {
    uint16_t bufIdx = 0;
    char c;

    // add to the buffer until run out of room or EOL is recived
    do {
        c = UART_getc();

        p_buffer[bufIdx++] = c;
    } while ((bufIdx < bufferLength) && (c != '\r') && (c != '\n'));

    // Ensure that line terminations are correct
    if (c == '\r') {
        p_buffer[bufIdx-1] = '\n';
    }

    p_buffer[bufIdx] = '\0';

    return bufIdx; 
}

uint16_t UART_get_line_echo(char* p_buffer, uint16_t bufferLength) {
    uint16_t bufIdx = 0;
    char c;

    // add to the buffer until run out of room or EOL is recived
    do {
        c = UART_getc();

        if (c == '\r') {
            UART_putc('\n');
        } else {
            UART_putc(c);
        }

        p_buffer[bufIdx++] = c;
    } while ((bufIdx < bufferLength) && (c != '\r') && (c != '\n'));

    // Ensure that line terminations are correct
    if (c == '\r') {
        p_buffer[bufIdx-1] = '\n';
    }

    p_buffer[bufIdx] = '\0';

    return bufIdx; 
}