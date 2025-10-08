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

#define DISP_OFF_BYTE 0x00
#define DISP_ON_BYTE 0x08

#define DATA_BYTE 0x40
#define CONTROL_BYTE 0x80
#define ADDR_BYTE 0xC0

#define WRITE_DATA_BYTE 0x00
#define AUTO_ADDRESS_BYTE 0x00
#define NORMAL_MODE_BYTE 0x00
#define TEST_MODE_BYTE 0x08
#define FIXED_ADDRESS_BYTE 0x04

// Digits
const uint8_t HexTo7Seg[40] =
{
  0x3F, // 0
  0x06, // 1
  0x5B, // 2
  0x4F, // 3
  0x66, // 4
  0x6D, // 5
  0x7D, // 6
  0x07, // 7
  0x7F, // 8
  0x6F, // 9
  0x77, // A
  0x7c, // b
  0x39, // C
  0x5E, // d
  0x79, // E
  0x71, // F
  0x3D, // g
  0x76, // H
  0x05, // i
  0x0D, // j
  0x36, // k
  0x38, // L
  0x00, // m
  0x54, // n
  0x5C, // o
  0x73, // P
  0x67, // q
  0x50, // r
  0x6D, // S
  0x70, // t
  0x3E, // U
  0x1C, // u
  0x00, // w
  0x00, // x
  0x66, // y
  0x6D, // z
  0x08, // _
  0x40, // -
  0x01  // Overscore
};

static void send_start(struct TM1638Device device) {
    GPIO_set_output(device.stbPin, false);
    GPIO_pin_init(device.stbPin, OUTPUT);
}

static void send_byte(struct TM1638Device device, uint8_t byte) {
    GPIO_pin_init(device.dataPin, OUTPUT);
    GPIO_pin_init(device.clockPin, OUTPUT);

    for (uint8_t i = 0; i < 8; i++) {
        GPIO_set_output(device.clockPin, false);
        delay_us(BIT_TIME_US / 2);
        GPIO_set_output(device.dataPin, byte & (1 << (i)));
        GPIO_set_output(device.clockPin, true);
        delay_us(BIT_TIME_US / 2);
    }
}

static void send_stop(struct TM1638Device device) {
    GPIO_set_output(device.stbPin, true);
    GPIO_pin_init(device.stbPin, OUTPUT);
}

int tm1638_init(struct TM1638Device* device, pin_t stbPin, pin_t dataPin, pin_t clockPin) {
    device->stbPin = stbPin;
    device->dataPin = dataPin;
    device->clockPin = clockPin;
    device->_dots = 0;
    device->_digits_enables = 0xFF;
    device->_brightness = 0x0;

    GPIO_pin_init(device->dataPin, INPUT_NO_PULLUP);
    GPIO_pin_init(device->clockPin, INPUT_NO_PULLUP);
    GPIO_pin_init(device->stbPin, OUTPUT);
    GPIO_set_output(device->stbPin, true);

    int result = 0;
    send_start(*device);
    send_byte(*device, CONTROL_BYTE | DISP_ON_BYTE | 0x0);
    send_stop(*device);

    delay_ms(200);

    send_start(*device);
    send_byte(*device, CONTROL_BYTE | DISP_OFF_BYTE);
    send_stop(*device);

    delay_ms(200);

    send_start(*device);
    send_byte(*device, CONTROL_BYTE | DISP_ON_BYTE | 0x0);
    send_stop(*device);

    return result;
}

void tm1638_enable_dot(struct TM1638Device* device, uint8_t digit, bool enable) {
    if (enable) {
        device->_dots |= (1 << digit);
    } else {
        device->_dots &= ~(1 << digit);
    }
}

void tm1638_enable_digit(struct TM1638Device* device, uint8_t digit, bool enable) {
    if (enable) {
        device->_digits_enables |= (1 << digit);
    } else {
        device->_digits_enables &= ~(1 << digit);
    }
}

int tm1638_write_digits(struct TM1638Device* device, uint8_t startingDigit, uint8_t values[], uint8_t valueLen) {
    // printf("Writing startingDigit %d with value 0x%x\n", startingDigit, value);

    uint8_t data = DATA_BYTE | WRITE_DATA_BYTE | AUTO_ADDRESS_BYTE | NORMAL_MODE_BYTE;

    send_start(*device);
    send_byte(*device, data);
    send_stop(*device);


    send_start(*device);
    send_byte(*device, ADDR_BYTE | (startingDigit * 2));

    for (uint8_t i = 0; i < valueLen; i++) {
        uint8_t valueSeg = HexTo7Seg[values[i]];

        if ((device->_dots >> (startingDigit + i)) & 1) {
            valueSeg |= (1 << 7);
        }
        if (!((device->_digits_enables >> (startingDigit + i)) & 1)) {
            valueSeg = 0;
        }

        send_byte(*device, valueSeg);
        send_byte(*device, 0x0);
    }

    send_stop(*device);

    return 0;
}

int tm1638_write(struct TM1638Device* device, uint32_t value, char* format) {
    // printf("Attempting to write to \"%s\" value: 0x%lx\n", device.name, value);
    uint8_t digits[6] = { 0 };
    if ((char)(format[1]) == 'x') {
        // Hex
        for (uint8_t i = 6; i > 0; i--) {
            digits[i - 1] = value % 16;
            value /= 16;
        }
    } else {
        // decimal
        for (uint8_t i = 6; i > 0; i--) {
            digits[i - 1] = value % 10;
            value /= 10;
        }
    }

    tm1638_write_digits(device, 0, digits, 6);

    return 0;
}

void tm1638_reset(struct TM1638Device* device) {
    device->_dots = 0;
    device->_digits_enables = 0xFF;

    tm1638_set_display_state(device, true);
}

void tm1638_set_display_state(struct TM1638Device* device, bool state) {
    send_start(*device);
    if (state) {
        send_byte(*device, CONTROL_BYTE | DISP_ON_BYTE | device->_brightness);
    } else {
        send_byte(*device, CONTROL_BYTE | DISP_OFF_BYTE);
    }
    send_stop(*device);
}

void tm1638_set_brightness(struct TM1638Device* device, uint8_t brightness) {
    device->_brightness = brightness;
    send_start(*device);
    send_byte(*device, CONTROL_BYTE | DISP_ON_BYTE | device->_brightness);
    send_stop(*device);
}

void tm1638_write_string(struct TM1638Device* device, char* string) {
    uint8_t i = 0;
    uint8_t values[6] = {0};

    while (string[i] != 0) {
        if (string[i] >= 'A' && string[i] <= 'Z') {
            values[i] = ((uint8_t)string[i] - 65) + 10;
        } else if (string[i] >= '0' && string[i] <= '9') {
            values[i] = (uint8_t)string[i] - 48;
        } else {
            tm1638_enable_digit(device, i, false);
        }
        i++;
    }

    tm1638_write_digits(device, 0, values, 6);
}