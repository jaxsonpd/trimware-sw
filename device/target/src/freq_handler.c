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
#include "custom_can_protocol/packet_handler.h"

#include "freq_info.h"
#include "device_select.h"

#include "freq_handler.h"

int freq_handler_init(void) {
    freq_info_init();

    return 0;
}

uint16_t freq_handler_packet_assemble(uint8_t* buffer) {
    freqType_t type = freq_handler_convert_to_type(device_select_get());
    
    freq_t standbyFreq = 0;
    freq_t activeFreq = 0;
    
    standbyFreq = freq_info_get(type, STANDBY_FREQ);
    activeFreq = freq_info_get(type, ACTIVE_FREQ);

    uint8_t bufferIndex = 0;

    buffer[bufferIndex++] = (uint8_t)type;

    buffer[bufferIndex++] = (standbyFreq >> 24) & 0xFF;
    buffer[bufferIndex++] = (standbyFreq >> 16) & 0xFF;
    buffer[bufferIndex++] = (standbyFreq >> 8) & 0xFF;
    buffer[bufferIndex++] = (standbyFreq >> 0) & 0xFF;

    buffer[bufferIndex++] = (activeFreq >> 24) & 0xFF;
    buffer[bufferIndex++] = (activeFreq >> 16) & 0xFF;
    buffer[bufferIndex++] = (activeFreq >> 8) & 0xFF;
    buffer[bufferIndex++] = (activeFreq >> 0) & 0xFF;

    return bufferIndex;
}

freqType_t freq_handler_convert_to_type(uint8_t value) {
    switch (value) {
        case 0x00: return COM1;
        case 0x01: return COM2;
        case 0x02: return NAV1;
        case 0x03: return NAV2;
        case 0x04: return ADF;
        case 0x05: return DME;
        case 0x06: return XPDR;
        default: return COM1;
    }
}

bool freq_handler_update(void) {
    freqType_t type = freq_handler_convert_to_type(device_select_get());
    
    freq_info_check_swap(type);

    return freq_info_update(type);
}

packetProcessingResult_t freq_handler_packet_cb(uint8_t* payload, uint16_t payloadLen) {
    freq_t standbyFreq = 0;
    freq_t activeFreq = 0;

    freqType_t type = (freqType_t)payload[0];

    standbyFreq = ((uint32_t)payload[1] << 24);
    standbyFreq |= ((uint32_t)payload[2] << 16);
    standbyFreq |= ((uint32_t)payload[3] << 8);
    standbyFreq |= ((uint32_t)payload[4] << 0);
    activeFreq = ((uint32_t)payload[5] << 24);
    activeFreq |= ((uint32_t)payload[6] << 16);
    activeFreq |= ((uint32_t)payload[7] << 8);
    activeFreq |= ((uint32_t)payload[8] << 0);

    freq_info_set(type, ACTIVE_FREQ, activeFreq);
    freq_info_set(type, STANDBY_FREQ, standbyFreq);

    return PROCESS_COMPLETE;
}