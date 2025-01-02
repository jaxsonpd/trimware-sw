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

#include "freq_display.h"

/** 
 * @brief Initialise the frequency display drivers
 * 
 * @return 0 if successful
 */
int freq_display_init(void) {
    printf("Initialising display driver\n");
    return 0;
}

/** 
 * @brief Update the frequency display
 * 
 * @return 0 if successful
 */
int freq_display_update(void* params) {
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
int freq_display_process(uint8_t* payloadBuf, uint16_t length) {
    printf("Processing Display Driver\n");
    return 0;
}

process_t freqDisplayProcess = {
    .identifier = 0x00,
    .name = "Freq Display Process",
    .updatePeriod = 1,
    .init = freq_display_init,
    .update = freq_display_update,
    .process_packet = freq_display_process
};


