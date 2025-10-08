#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


#include "avr_extends/GPIO.h"
#include "avr_extends/delay.h"
#include "avr_extends/UART.h"

pin_t pin13;

int main(void) {
    pin13 = PIN(PORTD, 6);
    GPIO_pin_init(pin13, OUTPUT);

    while (true) {
        GPIO_set_output(pin13, true);
        delay_s(1);
        GPIO_set_output(pin13, false);
        delay_s(1);
    }
}