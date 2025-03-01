/** 
 * @file freq_handler.h
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2025-03-01
 * @brief Handle storage and conversion of frequency values
 */


#ifndef FREQ_HANDLER_H
#define FREQ_HANDLER_H


#include <stdint.h>
#include <stdbool.h>

typedef struct FrequencyValue {
    uint16_t freqMHz; // The megahertz portion of the frequency (3 digits)
    uint16_t freqKHz; // The kilohertz portion of the frequency (3 digits)
} freq_t;

typedef enum PossibleFrequencies_e {
    STANDBY_FREQ,
    ACTIVE_FREQ
} freqType_t;

/** 
 * @brief Initialise the frequency handling module
 * 
 * @return 0 if successful
 */
int freq_info_init(void);

/** 
 * @brief Get the current frequency
 * @param freqType the frequency to get
 * 
 * @return a frequency struct with that current frequency
 */
freq_t freq_info_get(freqType_t freqType);

/** 
 * @brief Set a frequency
 * @param freqType the frequency to set
 * @param freqValue the value to set
 *
 */
void freq_info_set(freqType_t freqType, freq_t freqValue);

/** 
 * @brief Swap the active and standby frequencies
 * 
 */
void freq_info_check_swap(void);


#endif // FREQ_HANDLER_H