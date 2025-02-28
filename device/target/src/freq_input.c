/** 
 * @file freq_input.c
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2025-02-28
 * @brief Implementation of a module to count the frequency from the encoders
 */


#include <stdint.h>
#include <stdbool.h>

#include <stdio.h>

#include "avr_extends/GPIO.h"
#include "freq_input.h"

#define FINE_BUTTON_PIN PIN(PORTD, 2)
#define FINE_CHA_PIN PIN(PORTD, 3)
#define FINE_CHB_PIN PIN(PORTD, 4)

int freq_input_init(void) {
    GPIO_pin_init(FINE_BUTTON_PIN, INPUT_PULLUP);
    GPIO_pin_init(FINE_CHA_PIN, INPUT_PULLUP);
    GPIO_pin_init(FINE_CHB_PIN, INPUT_PULLUP);

    return 0;
}

int8_t freq_input_get(FreqInputValues_t input) {
    if (input == FREQ_FINE_INPUT) {
        bool buttonValue = GPIO_get_state(FINE_BUTTON_PIN);
        bool fineCHA = GPIO_get_state(FINE_CHA_PIN);
        bool fineCHB = GPIO_get_state(FINE_CHB_PIN);

        printf("Fine Encoder Values: %d, %d, %d\n", buttonValue, fineCHA, fineCHB);
    }

    return 0;
}