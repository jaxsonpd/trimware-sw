/** 
 * @file freq_input.c
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2025-02-28
 * @brief Implementation of a module to count the frequency from the encoders
 */


#include <stdint.h>
#include <stdbool.h>

#include <stdio.h>

#include <avr/interrupt.h>

#include "avr_extends/GPIO.h"
#include "freq_input.h"

#define STEPS_PER_CLICK 2 // number of encoder steps per division on encoder

#define FINE_CHA_PIN_NUM 3
#define FINE_CHB_PIN_NUM 4

#define FINE_BUTTON_PIN PIN(PORTD, 2)
#define FINE_CHA_PIN PIN(PORTD, FINE_CHA_PIN_NUM)
#define FINE_CHB_PIN PIN(PORTD, FINE_CHB_PIN_NUM)

#define COURSE_BUTTON_PIN PIN(PORTD, 5)
#define COURSE_CHA_PIN PIN(PORTD, 6)
#define COURSE_CHB_PIN PIN(PORTD, 7)

// Storage for changes made to the fine encoder
static volatile int8_t fineChange = 0;

ISR(PCINT2_vect) {
    static bool fineCHA_prev = false;
    static bool fineCHB_prev = false;

    bool fineCHA = GPIO_get_state(FINE_CHA_PIN);
    bool fineCHB = GPIO_get_state(FINE_CHB_PIN);

    if ((fineCHA != fineCHA_prev) || (fineCHB != fineCHB_prev)) {
        if (fineCHB != fineCHB_prev) {
            if (fineCHA == fineCHB_prev) fineChange++; // B leads A so clockwise
            else fineChange--; // A leads B so anti-clockwise
            fineCHB_prev = fineCHB;
        } else if(fineCHA != fineCHA_prev) {
            if (fineCHB == fineCHA_prev) fineChange--; // A leads B so anti-clockwise
            else fineChange++; // B leads A so clockwise
            fineCHA_prev = fineCHA;
        }
    }

}

int freq_input_init(void) {
    GPIO_pin_init(FINE_BUTTON_PIN, INPUT_PULLUP);
    GPIO_pin_init(FINE_CHA_PIN, INPUT_PULLUP);
    GPIO_pin_init(FINE_CHB_PIN, INPUT_PULLUP);

    GPIO_pin_init(COURSE_BUTTON_PIN, INPUT_PULLUP);
    GPIO_pin_init(COURSE_CHA_PIN, INPUT_PULLUP);
    GPIO_pin_init(COURSE_CHB_PIN, INPUT_PULLUP);

    // Enable interrupts
    PCMSK2 |= (1 << FINE_CHA_PIN_NUM) | (1 << FINE_CHB_PIN_NUM);

    PCICR |= (1 << PCIE2); // Enable the PCINT23..16 pin interrupt

    return 0;
}

int8_t freq_input_get(FreqInputValues_t input) {
    int8_t returnVal = 0;
    if (input == FREQ_FINE_INPUT) {
        returnVal = fineChange;
        returnVal = fineChange / STEPS_PER_CLICK;
        fineChange = 0;
    }

    return returnVal;
}