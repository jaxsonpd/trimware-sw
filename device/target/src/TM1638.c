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
  0x6F, // g
  0x3D, // G
  0x74, // h
  0x76, // H
  0x05, // i
  0x06, // I
  0x0D, // j
  0x30, // l
  0x38, // L
  0x54, // n
  0x37, // N
  0x5C, // o
  0x3F, // O
  0x73, // P
  0x67, // q
  0x50, // r
  0x6D, // S
  0x78, // t
  0x1C, // u
  0x3E, // U
  0x66, // y
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

int tm1638_init(struct TM1638Device *device) {
    GPIO_pin_init(device->dataPin, INPUT_NO_PULLUP);
    GPIO_pin_init(device->clockPin, INPUT_NO_PULLUP);
    GPIO_pin_init(device->stbPin, OUTPUT);
    GPIO_set_output(device->stbPin, true);

    int result = 0;
    send_start(*device);
    send_byte(*device, CONTROL_BYTE | DISP_ON_BYTE | 0x0);
    send_stop(*device);

    delay_ms(1000);

    send_start(*device);
    send_byte(*device, CONTROL_BYTE | DISP_OFF_BYTE);
    send_stop(*device);

    delay_ms(1000);

    send_start(*device);
    send_byte(*device, CONTROL_BYTE | DISP_ON_BYTE | 0x0);
    send_stop(*device);

    return result;
}

void tm1638_dot(struct TM1638Device *device, uint8_t digit, bool enable) {
    if (enable) {
        device->_dots |= (1 << digit);
    } else {
        device->_dots &= ~(1 << digit);
    }
}

int tm1638_write_digits(struct TM1638Device *device, uint8_t startingDigit, uint8_t values[], uint8_t valueLen) {
    // printf("Writing startingDigit %d with value 0x%x\n", startingDigit, value);

    uint8_t data = DATA_BYTE | WRITE_DATA_BYTE | AUTO_ADDRESS_BYTE | NORMAL_MODE_BYTE;

    send_start(*device);
    send_byte(*device, data);
    send_stop(*device);


    send_start(*device);
    send_byte(*device, ADDR_BYTE | (startingDigit*2));

    for (uint8_t i = 0; i < valueLen; i++) {
        uint8_t valueSeg = HexTo7Seg[values[i]];
        if ((device->_dots >> (startingDigit + i)) & 1) {
            valueSeg |= (1 << 7);
        }
        send_byte(*device, valueSeg);
        send_byte(*device, 0x0);
    }

    send_stop(*device);
    
    return 0;
}

int tm1638_write(struct TM1638Device *device, uint32_t value, char *format) {
    // printf("Attempting to write to \"%s\" value: 0x%lx\n", device.name, value);
    uint8_t digits[6] = {0};
    if ((char)format[1] == (char)"x") {
        // Hex
        
        for (uint8_t i = 6; i > 0; i--) {
            digits[i-1] = value % 16;
            value /= 16;
        }
        

    } else {
        for (uint8_t i = 6; i > 0; i--) {
            digits[i-1] = value % 10;
            value /= 10;
        }
    }

    tm1638_write_digits(device, 0, digits, 6);

    return 0;
}

// static int get_ack(struct TM1638Device device) {
//     int result = 0;
//     GPIO_set_output(device.dataPin, false);
//     GPIO_pin_init(device.dataPin, INPUT_NO_PULLUP);
//     delay_us(BIT_TIME_US / 2);
//     GPIO_set_output(device.clockPin, true);
//     delay_us(BIT_TIME_US / 4);
//     result = GPIO_get_state(device.dataPin);
//     delay_us(BIT_TIME_US / 4);
//     GPIO_set_output(device.clockPin, false);
//     GPIO_set_output(device.dataPin, false);
//     GPIO_pin_init(device.dataPin, OUTPUT);

//     return result;
// }