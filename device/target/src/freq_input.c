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
#include "pin.h"

#include "freq_input.h"


#define STEPS_PER_CLICK 2 // number of encoder steps per division on encoder


// Storage for changes made to the fine encoder
static volatile int8_t fineChange = 0;
static volatile int8_t coarseChange = 0;
static volatile FreqButtonState_t fineButtonState = FREQ_BUTTON_UP;

ISR(PCINT2_vect) {
    static bool fineCHA_prev = false;
    static bool fineCHB_prev = false;
    static bool coarseCHA_prev = false;
    static bool coarseCHB_prev = false;
    static bool fineButton_prev = !BUTTON_DOWN_VALUE;

    bool fineButton = GPIO_get_state(FINE_BUTTON_PIN);

    if (fineButton != fineButton_prev) {
        if (fineButtonState == FREQ_BUTTON_UP) {
            fineButtonState = FREQ_BUTTON_UP_DOWN;
        } else {
            fineButtonState = FREQ_BUTTON_DOWN_UP;
        }
        fineButton_prev = fineButton;
        return;
    }

    bool fineCHA = GPIO_get_state(FINE_CHA_PIN);
    bool fineCHB = GPIO_get_state(FINE_CHB_PIN);

    if ((fineCHA != fineCHA_prev) || (fineCHB != fineCHB_prev)) {
        if (fineCHB != fineCHB_prev) {
            if (fineCHA == fineCHB_prev) fineChange--; // B leads A so clockwise
            else fineChange++; // A leads B so anti-clockwise
            fineCHB_prev = fineCHB;
        } else if (fineCHA != fineCHA_prev) {
            if (fineCHB == fineCHA_prev) fineChange++; // A leads B so anti-clockwise
            else fineChange--; // B leads A so clockwise
            fineCHA_prev = fineCHA;
        }
        return;
    }

    bool coarseCHA = GPIO_get_state(COARSE_CHA_PIN);
    bool coarseCHB = GPIO_get_state(COARSE_CHB_PIN);

    if ((coarseCHA != coarseCHA_prev) || (coarseCHB != coarseCHB_prev)) {
        if (coarseCHB != coarseCHB_prev) {
            if (coarseCHA == coarseCHB_prev) coarseChange--; // B leads A so clockwise
            else coarseChange++; // A leads B so anti-clockwise
            coarseCHB_prev = coarseCHB;
        } else if (coarseCHA != coarseCHA_prev) {
            if (coarseCHB == coarseCHA_prev) coarseChange++; // A leads B so anti-clockwise
            else coarseChange--; // B leads A so clockwise
            coarseCHA_prev = coarseCHA;
        }
        return;
    }

}

int freq_input_init(void) {
    GPIO_pin_init(FINE_BUTTON_PIN, INPUT_PULLUP);
    GPIO_pin_init(FINE_CHA_PIN, INPUT_PULLUP);
    GPIO_pin_init(FINE_CHB_PIN, INPUT_PULLUP);

    GPIO_pin_init(COARSE_BUTTON_PIN, INPUT_PULLUP);
    GPIO_pin_init(COARSE_CHA_PIN, INPUT_PULLUP);
    GPIO_pin_init(COARSE_CHB_PIN, INPUT_PULLUP);

    // Enable interrupts
    PCMSK2 |= (1 << FINE_CHA_PIN_NUM) | (1 << FINE_CHB_PIN_NUM)
        | (1 << FINE_BUTTON_PIN_NUM) | (1 << COARSE_CHA_PIN_NUM)
        | (1 << COARSE_CHB_PIN_NUM) | (1 << COARSE_BUTTON_PIN_NUM);

    PCICR |= (1 << PCIE2); // Enable the PCINT23..16 pin interrupt

    return 0;
}

int8_t freq_input_get(FreqInputSources_t input) {
    int8_t returnVal = 0;
    volatile int8_t* change = &fineChange;

    if (input == FREQ_FINE_INPUT) {
        returnVal = fineChange;
        change = &fineChange;
    } else if (input == FREQ_COURSE_INPUT) {
        returnVal = coarseChange;
        change = &coarseChange;
    } else {
        return 0;
    }

    if ((returnVal % 2) == 1) {
        returnVal = (returnVal + 1) / STEPS_PER_CLICK;
        *change = -1;
    } else if ((returnVal % 2) == -1) {
        returnVal = (returnVal - 1) / STEPS_PER_CLICK;
        *change = 1;
    } else {
        returnVal = returnVal / STEPS_PER_CLICK;
        *change = 0;
    }

    return returnVal;
}

FreqButtonState_t freq_input_button_get(FreqInputSources_t button) {
    volatile FreqButtonState_t* state = &fineButtonState;

    if (button == FREQ_FINE_INPUT) {
        state = &fineButtonState;
    }

    cli();
    if (*state == FREQ_BUTTON_UP_DOWN) {
        *state = FREQ_BUTTON_DOWN;
        return FREQ_BUTTON_UP_DOWN;
    } else if (*state == FREQ_BUTTON_DOWN_UP) {
        *state = FREQ_BUTTON_UP;
        return FREQ_BUTTON_DOWN_UP;
    }
    sei();
    return *state;
}