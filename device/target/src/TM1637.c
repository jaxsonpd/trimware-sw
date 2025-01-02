/** 
 * @file TM1637.c
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2025-01-02
 * @brief Implementation for the TM1637 8 digit display module driver
 */


#include <stdint.h>
#include <stdbool.h>

#include <stdio.h>

#include "avr_extends/GPIO.h"
#include "avr_extends/delay.h"

#include "TM1637.h"

#define BIT_TIME_US 1000 // Time taken for one bit

static void write_start(struct TM1637Device device) {
    GPIO_set_output(device.clockPin, true);
    GPIO_set_output(device.dataPin, true);
    delay_us(BIT_TIME_US/2);

    GPIO_set_output(device.dataPin, false);
    delay_us(BIT_TIME_US/2);
    GPIO_set_output(device.clockPin, false);
}

static void write_byte(struct TM1637Device device, uint8_t byte) {  
    GPIO_set_output(device.clockPin, false);
  
    for (uint8_t i = 0; i < 8; i++) {
        delay_us(BIT_TIME_US/4);
        GPIO_set_output(device.dataPin, byte & (1 << (7-i)));
        delay_us(BIT_TIME_US/4);
        GPIO_set_output(device.clockPin, true);
        delay_us(BIT_TIME_US/2);
        GPIO_set_output(device.clockPin, false);
    }
}

static void write_stop(struct TM1637Device device) {
    GPIO_set_output(device.clockPin, false);
    GPIO_set_output(device.dataPin, false);
    delay_us(BIT_TIME_US/2);

    GPIO_set_output(device.clockPin, true);
    delay_us(BIT_TIME_US/2);
    GPIO_set_output(device.dataPin, true);
}

static int get_ack(struct TM1637Device device) {
    int result = 0;
    GPIO_set_output(device.dataPin, false);
    GPIO_pin_init(device.dataPin, INPUT_NO_PULLUP);
    delay_us(BIT_TIME_US/2);
    GPIO_set_output(device.clockPin, true);
    delay_us(BIT_TIME_US/4);
    result = GPIO_get_state(device.dataPin);
    delay_us(BIT_TIME_US/4);
    GPIO_set_output(device.clockPin, false);
    GPIO_set_output(device.dataPin, false);
    GPIO_pin_init(device.dataPin, OUTPUT);

    return result;
}

int tm1637_init(struct TM1637Device device) {
    GPIO_set_output(device.clockPin, true);
    GPIO_set_output(device.dataPin, true);

    GPIO_pin_init(device.dataPin, OUTPUT);
    GPIO_pin_init(device.clockPin, OUTPUT);

    return 0;
}

int tm1637_write(struct TM1637Device device, uint32_t value) {
    printf("Attempting to write to \"%s\" value: 0x%lx\n", device.name, value);

    write_start(device);
    write_byte(device, 0b10010100);
    int result = get_ack(device);
    write_stop(device);

    printf("Completed write ack result = 0x%x\n", result);

    return 0;
}