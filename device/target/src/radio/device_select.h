/**
 * @file device_select.h
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2025-02-28
 * @brief Module to handle radio device selection
 */


#ifndef CHANNEL_SELECT_H
#define CHANNEL_SELECT_H


#include <stdint.h>
#include <stdbool.h>

#include "custom_can_protocol/packet_processing.h"

/**
 * @brief Initialise the channel select value
 *
 * @return 0 if successful
 */
int device_select_init(void);

/**
 * @brief Get the current selected channel
 *
 * @return The channel selected from 0 - 7
 */
uint8_t device_select_get(void);

/**
 * @brief Assemble a radio device select payload to be sent
 * @param buffer the buffer to load into
 *
 * @return the length of the buffer
 */
uint16_t device_select_packet_assemble(uint8_t* buffer);

/**
 * @brief Update the device select value
 *
 * @return True if the device has changed
 */
bool device_select_update(void);

/**
 * @brief A callback to handle incoming channel select set packets
 * @param payload the packet payload buffer
 * @param payloadLen the packet payload length
 *
 * @return the result of the processing
 */
packetProcessingResult_t device_select_packet_cb(uint8_t* payload, uint16_t payloadLen);


#endif // CHANNEL_SELECT_H