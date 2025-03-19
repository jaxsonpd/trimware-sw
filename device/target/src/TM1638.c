/**
 * @file TM1638.c
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2025-03-19
 * @brief Driver for the TM1638 LED display driver.
 */


#include <stdint.h>
#include <stdbool.h>

#include <stdio.h>

#include "avr_extends/GPIO.h"
#include "avr_extends/delay.h"

#include "TM1638.h"

#define BIT_TIME_US 1000 // Time taken for one bit

#define SET_WRITE_AUTO_ADDR_CMD 0b01000100 // Put the device into write data with fixed address mode
#define DISPLAY_ON_10_16 0b10001101 // Turn the display on and use 10/16 pulse widths
#define DISPLAY_OFF_1_16 0b10000000

// Digit registers
#define C0H 0b11000000
#define C1H 0b11000001
#define C2H 0b11000010
#define C3H 0b11000011
#define C4H 0b11000100
#define C5H 0b11000101

// Digits
#define D0 0b00111011
#define D1 0b00000110
#define D2 0b01011011
#define D3 0b01100110
#define D4 0b01100001
#define D5 0b00110011
#define D6 0b00110111
#define D7 0b00000000

static void send_start(struct TM1638Device device) {
    GPIO_pin_init(device.stbPin, INPUT_NO_PULLUP);
    GPIO_pin_init(device.clockPin, INPUT_NO_PULLUP);
    GPIO_pin_init(device.dataPin, INPUT_NO_PULLUP);

    delay_us(BIT_TIME_US / 2);

    GPIO_set_output(device.stbPin, false);
    GPIO_pin_init(device.stbPin, OUTPUT);
    GPIO_set_output(device.dataPin, false);
    GPIO_pin_init(device.dataPin, OUTPUT);

    delay_us(BIT_TIME_US / 2);
    GPIO_set_output(device.clockPin, false);
    GPIO_pin_init(device.clockPin, OUTPUT);


}

static void send_byte(struct TM1638Device device, uint8_t byte) {
    GPIO_pin_init(device.dataPin, OUTPUT);
    GPIO_pin_init(device.clockPin, OUTPUT);
    GPIO_set_output(device.clockPin, false);

    for (uint8_t i = 0; i < 8; i++) {
        delay_us(BIT_TIME_US / 4);
        GPIO_set_output(device.dataPin, byte & (1 << (i)));
        delay_us(BIT_TIME_US / 4);
        GPIO_set_output(device.clockPin, true);
        delay_us(BIT_TIME_US / 2);
        GPIO_set_output(device.clockPin, false);
    }
}

static void send_stop(struct TM1638Device device) {
    GPIO_set_output(device.clockPin, false);
    GPIO_set_output(device.stbPin, false);
    GPIO_set_output(device.dataPin, false);
    delay_us(BIT_TIME_US / 2);

    GPIO_pin_init(device.clockPin, INPUT_NO_PULLUP);
    delay_us(BIT_TIME_US / 2);
    GPIO_pin_init(device.stbPin, INPUT_NO_PULLUP);
    GPIO_pin_init(device.dataPin, INPUT_NO_PULLUP);
}

static int get_ack(struct TM1638Device device) {
    int result = 0;
    GPIO_set_output(device.dataPin, false);
    GPIO_pin_init(device.dataPin, INPUT_NO_PULLUP);
    delay_us(BIT_TIME_US / 2);
    GPIO_set_output(device.clockPin, true);
    delay_us(BIT_TIME_US / 4);
    result = GPIO_get_state(device.dataPin);
    delay_us(BIT_TIME_US / 4);
    GPIO_set_output(device.clockPin, false);
    GPIO_set_output(device.dataPin, false);
    GPIO_pin_init(device.dataPin, OUTPUT);

    return result;
}

int tm1638_init(struct TM1638Device device) {
    GPIO_pin_init(device.dataPin, INPUT_NO_PULLUP);
    GPIO_pin_init(device.clockPin, INPUT_NO_PULLUP);
    GPIO_pin_init(device.stbPin, OUTPUT);
    GPIO_set_output(device.stbPin, true);

    int result = 0;
    send_start(device);
    send_byte(device, DISPLAY_ON_10_16);
    result = get_ack(device);
    send_stop(device);
    if (result != 0) {
        printf("ERROR: Bad ack in display on setting error = 0x%x\n", result);
    }
    delay_ms(1000);

    send_start(device);
    send_byte(device, DISPLAY_OFF_1_16);
    result = get_ack(device);
    send_stop(device);
    if (result != 0) {
        printf("ERROR: Bad ack in data command setting error = 0x%x\n", result);
    }

    delay_ms(1000);

    send_start(device);
    send_byte(device, DISPLAY_ON_10_16);
    result = get_ack(device);
    send_stop(device);
    if (result != 0) {
        printf("ERROR: Bad ack in display on setting error = 0x%x\n", result);
    }

    return result;
}

int tm1638_write_digit(struct TM1638Device device, uint8_t digit, uint8_t value) {
    int result = 0;

    send_start(device);
    send_byte(device, DISPLAY_ON_10_16);
    result = get_ack(device);
    send_stop(device);
    if (result != 0) {
        printf("ERROR: Bad ack in display on setting error = 0x%x\n", result);
    }

    send_start(device);
    send_byte(device, SET_WRITE_AUTO_ADDR_CMD);
    result = get_ack(device);
    send_stop(device);
    if (result != 0) {
        printf("ERROR: Bad ack in display on setting error = 0x%x\n", result);
    }

    send_start(device);
    send_byte(device, 0b1100000 | (digit*2));
    send_byte(device, 0b0000000);
    // send_stop(device);

    switch (value) {
    case 0:
        send_byte(device, 0b00111111);
        break;
    case 1:
        send_byte(device, 0b00000110);
        break;
    case 2:
        send_byte(device, 0b01011011);
    default:
        break;
    }

    send_stop(device);
    
    return 0;
}

int tm1638_write(struct TM1638Device device, uint32_t value) {
    printf("Attempting to write to \"%s\" value: 0x%lx\n", device.name, value);

    int result = 0;
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            result = tm1638_write_digit(device, j, i);
            delay_ms(1000);
        }
        delay_ms(2000);
    }
    printf("Completed write ack result = 0x%x\n", result);

    return 0;
}