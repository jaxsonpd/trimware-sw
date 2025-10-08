/**
 * @file freq_input.h
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2025-02-28
 * @brief A module to handle counting of encoder values this module contains
 * the interput handler for the Port D pins.
 */


#ifndef FREQ_INPUT_H
#define FREQ_INPUT_H


#include <stdint.h>
#include <stdbool.h>

typedef enum FreqInputSource_e {
    FREQ_COURSE_INPUT,
    FREQ_FINE_INPUT
} FreqInputSources_t;

typedef enum FreqButtonState_e {
    FREQ_BUTTON_UP,
    FREQ_BUTTON_DOWN,
    FREQ_BUTTON_UP_DOWN, // Transition from up to down
    FREQ_BUTTON_DOWN_UP, // Transition from down to up
} FreqButtonState_t;

/**
 * @brief Initialise the frequency counting module and setup the required inputs
 *
 * @return 0 if successful
 */
int freq_input_init(void);

/**
 * @brief Get an encoder value
 * @param input the frequency input to return
 *
 * @return The number of steps rotated since last call
 */
int8_t freq_input_get(FreqInputSources_t input);

/**
 * @brief Get the current state of a frequency button
 * @param button the button to get the value of
 *
 * @return the state of the button
 */
FreqButtonState_t freq_input_button_get(FreqInputSources_t button);


#endif // FREQ_INPUT_H