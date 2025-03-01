/** 
 * @file freq_display.c
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2025-01-02
 * @brief Implementation for the frequency display driver that communicates
 * with the 6 segment displays.
 */


#include <stdint.h>
#include <stdbool.h>

#include <stdio.h>

#include "process.h"

#include "freq_info.h"
#include "freq_display.h"

#include "freq_process.h"

/** 
 * @brief Initialise the frequency display drivers
 * 
 * @return 0 if successful
 */
int freq_process_init(void) {
    printf("Initialising display driver\n");
    freq_display_init();
    return 0;
}

/** 
 * @brief Update the frequency display
 * 
 * @return 0 if successful
 */
int freq_process_update(void* params) {
    // printf("Updating display driver\n");
    return 0;
}

/** 
 * @brief Process an incoming packet
 * @param payloadBuf the payload buffer
 * @param length the length of the payload buffer
 *  
 * @return 0 if successful
 */
int freq_process_packet(uint8_t* payloadBuf, uint16_t length) {
    // printf("Processing Display Driver\n");
    // if (length != 12) {
    //     printf("ERROR: Frequency display packet wrong length. Got: 0x%x, expected: 12", length);
    //     return 1;
    // }

    // // Get active frequency
    // uint32_t activeFrequency = 0;

    // for (int i = 0; i < 6; i++) {
    //     if (payloadBuf[i] < '0' || payloadBuf[i] > '9') {
    //         return 2; // Error: Non-digit character encountered
    //     }
    //     activeFrequency = (activeFrequency << 4) | (payloadBuf[i] - '0');
    // }

    // // Get standby frequency
    // uint32_t standbyFrequency = 0;

    // for (int i = 6; i < 12; i++) {
    //     if (payloadBuf[i] < '0' || payloadBuf[i] > '9') {
    //         return 2; // Error: Non-digit character encountered
    //     }
    //     standbyFrequency = (standbyFrequency << 4) | (payloadBuf[i] - '0');
    // }

    // freq_info_set(ACTIVE_FREQ, activeFrequency);
    // freq_info_set(STANDBY_FREQ, standbyFrequency);

    // printf("Extracted Frequencies, Active: %lx, Standby: %lx\n", freq_info_get(ACTIVE_FREQ),
    //             freq_info_get(STANDBY_FREQ));

    // printf("Active digit 1 0x%x\n", freq_digit(freq_info_get(ACTIVE_FREQ), 1));

    // freq_display_write(ACTIVE_FREQ, freq_info_get(ACTIVE_FREQ));    

    return 0;
}

process_t freqDisplayProcess = {
    .identifier = 0x00,
    .name = "Freq Display Process",
    .updatePeriod = 1,
    .init = freq_process_init,
    .update = freq_process_update,
    .process_packet = freq_process_packet
};


