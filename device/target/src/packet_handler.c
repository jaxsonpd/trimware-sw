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

typedef enum packetStates_e {
    START_BYTE,
    CMD_BYTE,
    PACKET_LENGTH_BYTE,
    PACKET_DATA_BYTES,
    CRC_BYTES,
    END_BYTE,
    PACKET_COMPLETE
} packetState_t;

packetStatus_t packet_validate(uint8_t* packetBuffer) {
    uint8_t byte;
    uint8_t currentByte = 0;
    uint8_t packetLength = 0;

    packetState_t state = START_BYTE;

    while ((byte = packetBuffer[currentByte]) != 0) {
        switch (state) {
        case START_BYTE:
            if (byte != PACKET_START_BYTE) {
                return PACKET_SCHEMA_ERROR;
            } else if (currentByte != 0) {
                return PACKET_SCHEMA_ERROR;
            } else {
                state = CMD_BYTE;
            }
            break;

        case CMD_BYTE:
            if (byte == PACKET_START_BYTE || byte == PACKET_END_BYTE) {
                return PACKET_SCHEMA_ERROR;
            } else if (currentByte != 1) {
                return PACKET_SCHEMA_ERROR;
            }else {
                state = PACKET_LENGTH_BYTE;
            }
            break;

        case PACKET_LENGTH_BYTE:
            if (byte == PACKET_START_BYTE || byte == PACKET_END_BYTE) {
                return PACKET_SCHEMA_ERROR;
            } else if (currentByte != 2) {
                return PACKET_SCHEMA_ERROR;
            } else {
                state = PACKET_DATA_BYTES;
                packetLength = byte;
            }
            break;

        case PACKET_DATA_BYTES:
            for (uint8_t i = 0; i < packetLength; i++) {
                if (packetBuffer[currentByte+i] == PACKET_END_BYTE 
                    || packetBuffer[currentByte+i] == PACKET_START_BYTE
                    || packetBuffer[currentByte+i] == 0) {
                        return PACKET_LENGTH_ERROR;
                }
            }

            if (packetBuffer[currentByte+packetLength+2] != PACKET_END_BYTE) {
                return PACKET_LENGTH_ERROR;
            } else {
                state = CRC_BYTES;
                currentByte += packetLength-1;
            }
            break;

        case CRC_BYTES:
            state = END_BYTE;
            currentByte += 1;

            break;
        
        case END_BYTE:
            if (byte != PACKET_END_BYTE) {
                return PACKET_SCHEMA_ERROR;
            } else {
                state = PACKET_COMPLETE;

            }
            break;

        default:
            return PACKET_SCHEMA_ERROR;
            break;
        }

        currentByte++;
    }

    if (state != PACKET_COMPLETE) {
        return PACKET_SCHEMA_ERROR;
    }

    return PACKET_VALID;
}