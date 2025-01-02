/** 
 * @file freq_info.h
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2025-01-02
 * @brief Declarations for the frequency storage abstraction
 */


#ifndef FREQ_INFO_H
#define FREQ_INFO_H


#include <stdint.h>
#include <stdbool.h>

typedef enum FrequencyTypes_e {
    ACTIVE_FREQ,
    STANDBY_FREQ
} freqType_t;

typedef uint32_t freq_t;

/** 
 * @brief Get a digit from a frequency
 * @param frequency the frequency to process
 * @param digit the digit to get zero indexed MSB is 0
 * 
 * @return the digit of the frequency as an 8 bit int
 */
#define freq_digit(freq, digit) (uint8_t)((freq >> (4*(5-digit))) & 0xF)

/** 
 * @brief Set the active frequency
 * @param type the frequency to set
 * @param frequency the new frequency to set encoded as each digit of a uint32_t
 * 
 */
void freq_info_set(freqType_t type, freq_t frequency);

/** 
 * @brief Get the frequency
 * @param type the frequency to set
 * 
 * @returns the frequency encoded as each digit of a uint32_t
 */
freq_t freq_info_get(freqType_t type);


/** 
 * @brief Swap the active and standby frequencies
 * 
 */
void freq_info_swap(void);

#endif // FREQ_INFO_H

