/**
 * @file process_handler.h
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2024-12-31
 * @brief Declarations for the functions to handle the processors
 * on the device.
 */


#ifndef PROCESS_HANDLER_H
#define PROCESS_HANDLER_H


#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Update all the processes on the device
 *
 * @return the result of updating 0 if successful
 */
int update_processes(void);

/**
 * @brief process a given packet
 * @param identifier the indentifier byte
 * @param payloadBuffer the payload buffer
 * @param payloadLength the length of the payload
 *
 * @return the result of processing and excuting the packet 0 if successful
 */
int execute_packet_process(uint8_t identifier, char* payloadBuffer, uint8_t payloadLength);


#endif // PROCESS_HANDLER_H