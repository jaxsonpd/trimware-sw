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

#define COM_MINIMUM_FREQ 108000
#define COM_MAXIMUM_FREQ 137999

#define NAV_MINIMUM_FREQ 108000
#define NAV_MAXIMUM_FREQ 117950

#define MAX_XPDR_VALUE 7777

#define MHz_STEP 1
#define KHz_STEP 25
#define NAV_KHz_STEP 50

#define MHz_OFFSET 1000
#define KHz_OFFSET 1

freq_t com1ActiveFreq = 118000;
freq_t com1StandbyFreq = 118000;
freq_t com2ActiveFreq = 118000;
freq_t com2StandbyFreq = 118000;

freq_t nav1ActiveFreq = 118000;
freq_t nav1StandbyFreq = 118000;
freq_t nav2ActiveFreq = 118000;
freq_t nav2StandbyFreq = 118000;

freq_t adf1ActiveFreq = 118000;
freq_t adf1StandbyFreq = 118000;

freq_t dme1ActiveFreq = 118000;
freq_t dme1StandbyFreq = 118000;

freq_t xpdrValue = 7000;

/** 
 * @brief update a spesific frequency value
 * @param 
 * 
 * @return 
 */
void update_freq_value(freq_t* freq, int8_t fineAdjust, int8_t coarseAdjust, freq_t maxFreq, freq_t minFreq, uint32_t khz_step) {
    freq_t temp = *freq + (coarseAdjust * MHz_STEP) * MHz_OFFSET
                    + (fineAdjust * khz_step) * KHz_OFFSET;
    
    if (temp > maxFreq) {
        temp = minFreq + (temp - maxFreq + 1);
    } else if (temp < minFreq) {
        temp = maxFreq - (minFreq - temp - 1);
    }
    
    *freq = temp;    
}  

int freq_info_init(void) {
    int result = freq_input_init();

    return result;
}

freq_t freq_info_get(freqType_t freqType, freqOption_t freqOption) {
    if (freqOption == ACTIVE_FREQ) {
        switch (freqType) {
        case COM1: return com1ActiveFreq;
        case COM2: return com2ActiveFreq;
        case NAV1: return nav1ActiveFreq;
        case NAV2: return nav2ActiveFreq;
        case XPDR: return xpdrValue;
        default:
            break;
        }
    } else if (freqOption == STANDBY_FREQ) {
        switch (freqType) {
        case COM1: return com1StandbyFreq;
        case COM2: return com2StandbyFreq;
        case NAV1: return nav1StandbyFreq;
        case NAV2: return nav2StandbyFreq;
        default:
            break;
        }
    }

    return 0;
}

uint32_t increment_octal(uint32_t octal, uint32_t increment) {
    uint32_t decimal = 0, power = 1, temp = octal;
    
    // Convert octal to decimal
    while (temp > 0) {
        decimal += (temp % 10) * power;
        temp /= 10;
        power *= 8;
    }
    
    // Increment in decimal
    decimal += increment;
    
    // Convert back to octal
    uint32_t new_octal = 0, place = 1;
    while (decimal > 0) {
        new_octal += (decimal % 8) * place;
        decimal /= 8;
        place *= 10;
    }
    
    return new_octal;
}

bool freq_info_update(freqType_t freqType) {
    int8_t fineAdjust = freq_input_get(FREQ_FINE_INPUT);
    int8_t coarseAdjust = freq_input_get(FREQ_COURSE_INPUT);
    
    uint32_t value;
    switch (freqType) {
    case COM1:
        update_freq_value(&com1StandbyFreq, fineAdjust, coarseAdjust,
            COM_MAXIMUM_FREQ, COM_MINIMUM_FREQ, KHz_STEP);
        break;
    case COM2:
        update_freq_value(&com2StandbyFreq, fineAdjust, coarseAdjust,
            COM_MAXIMUM_FREQ, COM_MINIMUM_FREQ, KHz_STEP);
        break;
    case NAV1:
        update_freq_value(&nav1StandbyFreq, fineAdjust, coarseAdjust,
            NAV_MAXIMUM_FREQ, NAV_MINIMUM_FREQ, NAV_KHz_STEP);
        break;
    case NAV2:
        update_freq_value(&nav2StandbyFreq, fineAdjust, coarseAdjust,
            NAV_MAXIMUM_FREQ, NAV_MINIMUM_FREQ, NAV_KHz_STEP);
        break;
    case XPDR:
        value = increment_octal((uint32_t)xpdrValue, fineAdjust);
        
        if (value > MAX_XPDR_VALUE) {
            value -= MAX_XPDR_VALUE;
            if (value > MAX_XPDR_VALUE) {
                value = 0;
            }
        }

        xpdrValue = value;

        break;
    default:
        break;
    }

    return fineAdjust != 0 || coarseAdjust != 0;
}

void freq_info_set(freqType_t freqType, freqOption_t freqOption, freq_t freqValue) {
    // Clear inputs as they were based on an old frequency
    freq_input_get(FREQ_FINE_INPUT);
    freq_input_get(FREQ_COURSE_INPUT);

    if (freqOption == ACTIVE_FREQ) {
        switch (freqType){
        case COM1: com1ActiveFreq = freqValue; break;
        case COM2: com2ActiveFreq = freqValue; break;
        case NAV1: nav1ActiveFreq = freqValue; break;
        case NAV2: nav2ActiveFreq = freqValue; break;
        
        default:
            break;
        }
    } else if (freqOption == STANDBY_FREQ) {
        switch (freqType){
        case COM1: com1StandbyFreq = freqValue; break;
        case COM2: com2StandbyFreq = freqValue; break;
        case NAV1: nav1StandbyFreq = freqValue; break;
        case NAV2: nav2StandbyFreq = freqValue; break;
        
        default:
            break;
        }
    }
}

void freq_info_check_swap(freqType_t freqType) {
    FreqButtonState_t freqButtonState = freq_input_button_get(FREQ_FINE_INPUT);
    if (freqButtonState != FREQ_BUTTON_UP_DOWN) {
        return;
    }

    freq_info_swap(freqType);
}

void freq_info_swap(freqType_t freqType) {
    freq_t temp = 0;

    switch (freqType) {
        case COM1:
            temp = com1ActiveFreq;
            com1ActiveFreq = com1StandbyFreq;
            com1StandbyFreq = temp;
            break;
        case COM2:
            temp = com2ActiveFreq;
            com2ActiveFreq = com2StandbyFreq;
            com2StandbyFreq = temp;
            break;
        case NAV1:
            temp = nav1ActiveFreq;
            nav1ActiveFreq = nav1StandbyFreq;
            nav1StandbyFreq = temp;
            break;
        case NAV2:
            temp = nav2ActiveFreq;
            nav2ActiveFreq = nav2StandbyFreq;
            nav2StandbyFreq = temp;
            break;
    
    default:
        break;
    }
}