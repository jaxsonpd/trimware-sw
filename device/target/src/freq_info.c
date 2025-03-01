/** 
 * @file freq_handler.c
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2025-03-01
 * @brief Handle storage and changing of the current frequencies for the device
 */


#include <stdint.h>
#include <stdbool.h>

#include <stdio.h>

#include "freq_input.h"

#include "freq_info.h"

#define MINIMUM_FREQ_MHz 108
#define MINIMUM_FREQ_KHz 000

#define MAXIMUM_FREQ_MHz 137
#define MAXIMUM_FREQ_KHz 999

#define DEFAULT_FREQ_MHz 118
#define DEFAULT_FREQ_KHz 000

#define MHz_STEP 1
#define KHz_STEP 25


freq_t activeFreq = {
    .freqKHz = DEFAULT_FREQ_KHz,
    .freqMHz = DEFAULT_FREQ_MHz,
};

freq_t standbyFreq = {
    .freqKHz = DEFAULT_FREQ_KHz,
    .freqMHz = DEFAULT_FREQ_MHz,
};

int freq_info_init(void) {
    int result = freq_input_init();

    return result;
}

freq_t freq_info_get(freqType_t freqType) {
    if (freqType == ACTIVE_FREQ) {
        return activeFreq;
    } else if (freqType == STANDBY_FREQ) {
        int8_t fineAdjust = freq_input_get(FREQ_FINE_INPUT);
        int8_t coarseAdjust = freq_input_get(FREQ_COURSE_INPUT);
        
        int16_t adjustedKHz = standbyFreq.freqKHz + fineAdjust*KHz_STEP;
        int16_t adjustedMHz = standbyFreq.freqMHz + coarseAdjust*MHz_STEP;
        
        if (adjustedKHz > MAXIMUM_FREQ_KHz) {
            standbyFreq.freqKHz = MINIMUM_FREQ_KHz + (adjustedKHz - MAXIMUM_FREQ_KHz - 1);
        } else if (adjustedKHz < MINIMUM_FREQ_KHz) {
            standbyFreq.freqKHz = MAXIMUM_FREQ_KHz - (MINIMUM_FREQ_KHz - adjustedKHz - 1);
        } else {
            standbyFreq.freqKHz = adjustedKHz;
        }

        if (adjustedMHz > MAXIMUM_FREQ_MHz) {
            standbyFreq.freqMHz = MINIMUM_FREQ_MHz + (adjustedMHz - MAXIMUM_FREQ_MHz - 1);
        } else if (adjustedMHz < MINIMUM_FREQ_MHz) {
            standbyFreq.freqMHz = MAXIMUM_FREQ_MHz - (MINIMUM_FREQ_MHz - adjustedMHz - 1);
        } else {
            standbyFreq.freqMHz = adjustedMHz;
        }

        return standbyFreq;
    } else {
        return (freq_t){0, 0};
    }
}

void freq_info_set(freqType_t freqType, freq_t freqValue) {
    // Clear inputs as they were based on an old frequency
    freq_input_get(FREQ_FINE_INPUT);
    freq_input_get(FREQ_COURSE_INPUT);

    if (freqType == ACTIVE_FREQ) {
        activeFreq = freqValue;
    } else if (freqType == STANDBY_FREQ) {
        standbyFreq = freqValue;
    }
}

void freq_info_check_swap(void) {
    FreqButtonState_t freqButtonState = freq_input_button_get(FREQ_FINE_INPUT);

    if (freqButtonState == FREQ_BUTTON_UP_DOWN) {
        freq_t temp = activeFreq;
        activeFreq = standbyFreq;
        standbyFreq = temp;
    }
}