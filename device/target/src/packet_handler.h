/** 
 * @file packet_decoder.h
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2024-12-27
 * @brief Declarations for the packet handling functionality including validation
 * extraction and compilation.
 */


#ifndef PACKET_DECODER_H
#define PACKET_DECODER_H


#include <stdint.h>
#include <stdbool.h>

typedef enum packetValidationStatus_e {
    PACKET_VALID,
    PACKET_LENGTH_ERROR,
    PACKET_CMD_ERROR,
    PACKET_CRC_ERROR,
    PACKET_FRAMING_ERROR,
} packetStatus_t;

/** 
 * @brief Check if the packet is valid
 * @param packetBuffer the packet string
 * 
 * @return the packet status
 */
packetStatus_t packet_validate(char* packetBuffer);


#endif // PACKET_DECODER_H