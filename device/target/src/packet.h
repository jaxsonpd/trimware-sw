/** 
 * @file packet.h
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2024-12-27
 * @brief Definitions for the packet schema
 */


#ifndef PACKET_H
#define PACKET_H


#include <stdint.h>
#include <stdbool.h>

#define PACKET_START_BYTE 0x7E
#define PACKET_END_BYTE 0x7E

#define PACKET_CRC_LENGTH 0x02

enum packetByteLocations_e {
    PACKET_IDENTIFIER_LOC = 0x01,
    PACKET_LENGTH_LOC = 0x02,
    PACKET_PAYLOAD_START_LOC = 0x03,
};

typedef enum packetIdentifiers_e {
    PACKET_UPDATE_FREQ = 0x01,
    PACKET_ENCODER_ADJ = 0x02,
    PACKET_SWAP_BT_STATE = 0x03,
    PACKET_ROTARY_SW_STATE = 0x04,
    PACKET_LED_TOGGLE = 0x05,
    PACKET_RESEND = 0xFE,
    PACKET_HEARTBEAT = 0xFF,
} packetIdentifier_t;


#endif // PACKET_H