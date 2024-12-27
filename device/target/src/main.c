
/**
 * @file main.c
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2024-12-23
 * @brief Main file for the flight simulator radio panel project
 */


#include <stdint.h>
#include <stdbool.h>


#include "avr_extends/GPIO.h"
#include "avr_extends/delay.h"


int main(void) {
    pin_t pin13 = PIN(PORTB, 5);

    GPIO_pin_init(pin13, OUTPUT);

    while (true) {
        GPIO_set_output(pin13, false);
        delay_us(500);

        GPIO_set_output(pin13, true);
        delay_us(500);

    }
    return 0;
}
