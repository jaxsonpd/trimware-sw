/** 
 * @file freq_info.c
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2025-01-02
 * @brief Implementation for the frequency storage module
 */


#include <stdint.h>
#include <stdbool.h>

#include "freq_info.h"

freq_t activeFrequency = 123456;
freq_t standbyFrequency = 123456;

void freq_info_set(freqType_t type, freq_t frequency) {
    switch (type) {
    case ACTIVE_FREQ:
        activeFrequency = frequency;
        break;
    
    case STANDBY_FREQ:
        standbyFrequency = frequency;
        break;

    default:
        break;
    }
}


freq_t freq_info_get(freqType_t type) {
    freq_t frequency = 0;
    switch (type) {
    case ACTIVE_FREQ:
        frequency = activeFrequency;
        break;
    
    case STANDBY_FREQ:
        frequency = standbyFrequency;
        break;

    default:
        break;
    }

    return frequency;
}

void freq_info_swap(void) {
    freq_t temp = activeFrequency;
    activeFrequency = standbyFrequency;
    standbyFrequency = temp;
}