/**
 * @file freq_display.h
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2025-01-02
 * @brief Abstraction for the frequency display module
 */


#ifndef FREQ_DISPLAY_H
#define FREQ_DISPLAY_H


#include <stdint.h>
#include <stdbool.h>

#include "freq_info.h"

/**
 * @brief Initialise the frequency display
 *
 * @return 0 if successful
 */
int freq_display_init(void);

/**
 * @brief Write a frequency to the display
 * @param type the display to write too
 * @param frequency the frequency to write
 *
 * @return 0 if successful
 */
int freq_display_write(freqOption_t type, freq_t frequency);


#endif // FREQ_DISPLAY_H