/**
 * @file GPIO.c
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2024-04-07
 * @brief A library to control the GPIO on the microcontroller
 */


#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>

#include "../avr_extends/GPIO.h"

void GPIO_pin_init(pin_t pin, pinType_t type) {
    // Set the DDR and PORT to match the type
    volatile uint8_t* DDR_pin = (uint8_t*)PINBANK(pin) - 1; // DDRX is one regs lower then PORTX 

    if (type == OUTPUT) {
        *DDR_pin |= (1 << PINNUM(pin));
    } else if (type == INPUT_PULLUP) {
        *DDR_pin &= ~(1 << PINNUM(pin));
        *(uint8_t*)PINBANK(pin) &= (1 << PINNUM(pin)); // set pullup
    } else if (type == INPUT_NO_PULLUP) {
        *DDR_pin &= ~(1 << PINNUM(pin));
        *(uint8_t*)PINBANK(pin) &= ~(1 << PINNUM(pin)); // set pullup
    }
}


void GPIO_set_output(pin_t pin, bool value) {
    if (value) {
        *(volatile uint8_t*)PINBANK(pin) |= (1 << PINNUM(pin));
    } else {
        *(volatile uint8_t*)PINBANK(pin) |= ~(1 << PINNUM(pin));
    }
}


bool GPIO_toggle_output(pin_t pin) {
    *(uint8_t*)PINBANK(pin) ^= (1 << PINNUM(pin));

    return *(uint8_t*)(PINBANK(pin) - 2); // PINx regs is 2 addresses lower
}

bool GPIO_getState(pin_t pin) {
    return *(uint8_t*)(PINBANK(pin) - 2); // PINx regs is 2 addresses lower
}