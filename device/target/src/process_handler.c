/**
 * @file process_handler.c
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2024-12-31
 * @brief Implementation for the handling of the processes on the device
 */


#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "process.h"
#include "flags.h"

#include "process_handler.h"

const uint16_t numProcesses = 1;
process_t processes[numProcesses] = { 0 };


int update_processes(void) {
    int updateResult = 0;
    for (uint16_t i = 0; i < numProcesses; i++) {
        int result = processes[i].update(NULL);
        if (result != 0) {
            updateResult |= 1 << i;
        }

        if (debug) {
            printf("Updating process: %s, result: 0x%x\n", processes[i].name, result);
        }

    }

    return updateResult;
}

int execute_packet_process(uint8_t identifier, char* payloadBuffer, uint8_t payloadLength) {
    int processResult = 0;
    bool handled = false;
    for (uint16_t i = 0; i < numProcesses; i++) {
        if (processes[i].identifier == identifier) {
            handled = true;
            processResult = processes[i].process_packet(payloadBuffer, payloadLength);
        }

        if (debug) {
            printf("Executing process: %s, result: 0x%x\n", processes[i].name, processResult);
        }
    }

    if (!handled) {
        return -1;
    }

    return processResult;
}