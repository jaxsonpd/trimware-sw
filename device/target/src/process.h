/** 
 * @file process.h
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2024-12-31
 * @brief Declarations for the helper structs for the various 
 * processes that run on the device (frequency update etc.)
 */


#ifndef PROCESS_H
#define PROCESS_H


#include <stdint.h>
#include <stdbool.h>

#include "packet.h"

typedef struct Process {
    packetIdentifier_t identifier; // The byte identifier for the process
    char* name; // The name of the process
    int (*process_packet)(char*, uint16_t); // A function to process an incoming packet
    int (*update)(void*); // The update function for the process called often
    uint64_t updatePeriod; // How often the process should be updated
} process_t;

#endif // PROCESS_H