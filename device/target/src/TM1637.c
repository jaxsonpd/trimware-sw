/** 
 * @file TM1637.c
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2025-01-02
 * @brief Implementation for the TM1637 8 digit display module driver
 */


#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

#include <stdio.h>

#include "avr_extends/GPIO.h"

#include "TM1637.h"

int tm1637_init(struct TM1637Device device) {
    GPIO_pin_init(device.dataPin, OUTPUT);
    GPIO_pin_init(device.clockPin, OUTPUT);

    return 0;
}

int tm1637_write(struct TM1637Device device, uint32_t value) {
    printf("Attempting to write to \"%s\" value: 0x%lx\n", device.name, value);


    return 0;
}