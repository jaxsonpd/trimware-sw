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

/// @brief The frequency of the radio in kHz
typedef uint32_t freq_t;

/// @brief The frequencies that are stored
typedef enum PossibleFreqTypes_e {
    COM1,
    COM2,
    NAV1,
    NAV2,
    DME,
    ADF,
    XPDR,
} freqType_t;

typedef enum PossibleFreqOptions_e {
    STANDBY_FREQ,
    ACTIVE_FREQ
} freqOption_t;

/**
 * @brief Initialise the frequency handling module
 *
 * @return 0 if successful
 */
int freq_info_init(void);

/**
 * @brief Get the current frequency
 * @param freqType the frequency to get
 * @param freqOption the option to get
 * 
 * @return a frequency value
 */
freq_t freq_info_get(freqType_t freqType, freqOption_t freqOption);

/** 
 * @brief Return the button status
 * 
 */


/** 
 * @brief Update the selected frequency
 * 
 * @return true if the frequency has changed
 */
bool freq_info_update(freqType_t freqType);

/**
 * @brief Set a frequency
 * @param freqType the frequency to get
 * @param freqOption the option to get
 * @param freqValue the value to set
 *
 */
void freq_info_set(freqType_t freqType, freqOption_t freqOption, freq_t freqValue);

/**
 * @brief Check to swap the active and standby frequencies
 * @param freqType the frequency to swap if true
 * 
 */
void freq_info_check_swap(freqType_t freqType);

/**
 * @brief Swap the active and standby frequencies
 * @param freqType the frequency to swap
 *
 */
void freq_info_swap(freqType_t freqType);


#endif // FREQ_HANDLER_H