/**
 * @file packet_handler.c
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2024-12-27
 * @brief Implementation for the packet handling functionality
 */


#include <stdint.h>
#include <stdbool.h>

#include "packet_handler.h"

#include "packet.h"

packetStatus_t packet_validate(uint8_t* packetBuffer) {
    uint8_t byte;
    uint8_t currentByte = 0;

    while ((byte = packetBuffer[currentByte]) != 0) {
        switch (currentByte)
        {
        case 0:
            if (currentByte != PACKET_START_BYTE) {
                return PACKET_SCHEMA_ERROR;
            }
            break;
        case 0:
            if (currentByte != PACKET_START_BYTE) {
                return PACKET_SCHEMA_ERROR;
            }
            break;
        
        default:
            break;
        }
    }

    return PACKET_SCHEMA_ERROR;
}