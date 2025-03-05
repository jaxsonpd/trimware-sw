/** 
 * @file freq_handler.c
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2025-03-05
 * @brief Handle the frequency input and storage functionality
 */


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "custom_can_protocol/packet_processing.h"

#include "freq_info.h"

#include "freq_handler.h"

freq_t standbyFreq_prev = {
    .freqKHz = 100,
    .freqMHz = 100
};

freq_t activeFreq_prev = {
    .freqKHz = 100,
    .freqMHz = 100
};

int freq_handler_init(void) {
    freq_info_init();

    standbyFreq_prev = freq_info_get(STANDBY_FREQ);
    activeFreq_prev = freq_info_get(ACTIVE_FREQ);

    return 0;
}

uint16_t freq_handler_packet_assemble(uint8_t *buffer) {

    freq_t standbyFreq = freq_info_get(STANDBY_FREQ);
    freq_t activeFreq = freq_info_get(ACTIVE_FREQ);

    uint8_t bufferIndex = 0;

    buffer[bufferIndex++] = (standbyFreq.freqMHz >> 8) & 0xFF;
    buffer[bufferIndex++] = standbyFreq.freqMHz & 0xFF;
    buffer[bufferIndex++] = (standbyFreq.freqKHz >> 8) & 0xFF;
    buffer[bufferIndex++] = standbyFreq.freqKHz & 0xFF;
    buffer[bufferIndex++] = (activeFreq.freqMHz >> 8) & 0xFF;
    buffer[bufferIndex++] = activeFreq.freqMHz & 0xFF;
    buffer[bufferIndex++] = (activeFreq.freqKHz >> 8) & 0xFF;
    buffer[bufferIndex++] = activeFreq.freqKHz & 0xFF;

    standbyFreq_prev = standbyFreq;
    activeFreq_prev = activeFreq;

    return bufferIndex;
}

bool freq_handler_freq_changed(void) {
    freq_info_check_swap();

    freq_t standbyFreq = freq_info_get(STANDBY_FREQ);
    freq_t activeFreq = freq_info_get(ACTIVE_FREQ);

    return (standbyFreq_prev.freqKHz != standbyFreq.freqKHz || 
        standbyFreq_prev.freqMHz != standbyFreq.freqMHz ||
        activeFreq_prev.freqKHz != activeFreq.freqKHz || 
        activeFreq_prev.freqMHz != activeFreq.freqMHz);
}

packetProcessingResult_t freq_handler_packet_cb(uint8_t *payload, uint16_t payloadLen) {
    freq_t standbyFreq = {0};
    freq_t activeFreq = {0};

    standbyFreq.freqMHz = (payload[0] << 8);
    standbyFreq.freqMHz |= payload[1];
    standbyFreq.freqKHz = (payload[2] << 8);
    standbyFreq.freqKHz |= payload[3];
    activeFreq.freqMHz = (payload[4] << 8);
    activeFreq.freqMHz |= payload[5];
    activeFreq.freqKHz = (payload[6] << 8);
    activeFreq.freqKHz |= payload[7];

    freq_info_set(ACTIVE_FREQ, activeFreq);
    freq_info_set(STANDBY_FREQ, standbyFreq);
    
    return PROCESS_COMPLETE;
}