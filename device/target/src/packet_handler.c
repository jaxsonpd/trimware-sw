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

static uint16_t calculate_crc16(const uint8_t *data, uint16_t length) {
    uint16_t crc = 0xFFFF; // Start with the initial CRC value
    const uint16_t polynomial = PACKET_CRC_POLYNOMIAL; // CRC-16-CCITT polynomial

    for (uint16_t i = 0; i < length; i++) {
        crc ^= (data[i] << 8); // XOR the byte into the high order byte of the CRC

        for (uint8_t bit = 0; bit < 8; bit++) {
            if (crc & 0x8000) { // Check if the MSB is set
                crc = (crc << 1) ^ polynomial;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc; // Return the computed CRC-16 value
}

packetStatus_t packet_validate(uint8_t* packetBuffer) {
    uint8_t byte;
    uint8_t currentByte = 0;
    uint8_t packetLength = 0;

    packetState_t state = START_BYTE;

    while (currentByte < 128 && state != PACKET_COMPLETE) {
        byte = packetBuffer[currentByte];
        switch (state) {
        case START_BYTE:
            if (byte != PACKET_START_BYTE) {
                return PACKET_SCHEMA_ERROR;
            } else if (currentByte != 0) {
                return PACKET_SCHEMA_ERROR;
            } else {
                state = CMD_BYTE;
                currentByte++;
            }
            break;

        case CMD_BYTE:
            if (byte == PACKET_START_BYTE || byte == PACKET_END_BYTE) {
                return PACKET_SCHEMA_ERROR;
            } else if (currentByte != 1) {
                return PACKET_SCHEMA_ERROR;
            }else {
                state = PACKET_LENGTH_BYTE;
                currentByte++;
            }
            break;

        case PACKET_LENGTH_BYTE:
            if (currentByte != 2) {
                return PACKET_SCHEMA_ERROR;
            } else {
                state = PACKET_DATA_BYTES;
                packetLength = byte;
                currentByte++;
            }
            break;

        case PACKET_DATA_BYTES:
            for (uint8_t i = 0; i < packetLength; i++) {
                if (packetBuffer[currentByte+i] == PACKET_END_BYTE 
                    || packetBuffer[currentByte+i] == PACKET_START_BYTE) {
                        return PACKET_LENGTH_ERROR;
                }
            }

            if (packetBuffer[currentByte+packetLength+2] != PACKET_END_BYTE) {
                return PACKET_LENGTH_ERROR;
            } else {
                state = CRC_BYTES;
                currentByte += packetLength;
            }
            break;

        case CRC_BYTES:
            uint16_t crc16 = calculate_crc16(&packetBuffer[PACKET_PAYLOAD_START_LOC], packetLength);
            uint16_t receivedCrc16 = packetBuffer[currentByte] << 8 | packetBuffer[currentByte+1];

            if (crc16 != receivedCrc16) {
                return PACKET_CRC_ERROR;
            } else {
                state = END_BYTE;
                currentByte += 2;
            }

            break;
        
        case END_BYTE:
            if (byte != PACKET_END_BYTE) {
                return PACKET_SCHEMA_ERROR;
            } else {
                state = PACKET_COMPLETE;
                currentByte++;
            }
            break;

        default:
            return PACKET_SCHEMA_ERROR;
            break;
        }
    }

    if (state != PACKET_COMPLETE) {
        return PACKET_SCHEMA_ERROR;
    }

    return PACKET_VALID;
}

packetState_t packet_compile(uint8_t* packetBuf, uint8_t* payloadBuf, uint8_t payloadLength, packetIdentifier_t packetIdent) {

    return PACKET_VALID;
}