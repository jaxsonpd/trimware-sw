/** 
 * @file freq_handler.h
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2025-03-05
 * @brief Handle frequency packet generation and processing
 */


#ifndef PACKET_HANDLER_H
#define PACKET_HANDLER_H

#include "custom_can_protocol/packet_processing.h"

#include <stdint.h>
#include <stdbool.h>

/** 
 * @brief Initialise the frequency handler
 * 
 * @return 0 if successful
 */
int freq_handler_init(void);

/** 
 * @brief Assemble a frequency handler payload to be sent
 * @param buffer the buffer to load into
 * 
 * @return the length of the buffer
 */
uint16_t freq_handler_packet_assemble(uint8_t *buffer);

/** 
 * @brief Return true if the frequency has updated since last call
 * 
 * @return true if frequency has changed
 */
bool freq_handler_freq_changed(void);

/** 
 * @brief A callback to handle incoming frequency set packets
 * @param payload the packet payload buffer
 * @param payloadLen the packet payload length
 * 
 * @return the result of the processing
 */
packetProcessingResult_t freq_handler_packet_cb(uint8_t *payload, uint16_t payloadLen);

#endif // PACKET_HANDLER_H