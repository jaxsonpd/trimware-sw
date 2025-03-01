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

typedef enum FreqInputValues_e {
    FREQ_COURSE_INPUT,
    FREQ_FINE_INPUT
} FreqInputValues_t;

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
int8_t freq_input_get(FreqInputValues_t input);


#endif // FREQ_INPUT_H