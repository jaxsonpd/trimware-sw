/**
 * @file process_handler.c
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2024-12-31
 * @brief Implementation for the handling of the processes on the device
 */


#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "process.h"
#include "flags.h"

#include "freq_display.h"

#include "process_handler.h"

#define NUM_PROCESSES 1
process_t *processes[NUM_PROCESSES] = { 0 };

int init_processes(void) {
    processes[0] = &freqDisplayProcess;

    int initResult = 0;
    for (uint8_t i = 0; i < NUM_PROCESSES; i++) {
        int result = processes[i]->init();

        if (result != 0) {
            initResult |= 1 << i;
        }

        if (debug) {
            printf("Initialising process: %s, result: 0x%x\n", processes[i]->name, result);
        }
    }

    return initResult;

}

int update_processes(void) {
    int updateResult = 0;
    for (uint16_t i = 0; i < NUM_PROCESSES; i++) {
        int result = processes[i]->update(NULL);
        if (result != 0) {
            updateResult |= 1 << i;
        }   

        // if (debug) {
        //     printf("Updating process: %s, result: 0x%x\n", processes[i]->name, result);
        // }

    }

    return updateResult;
}

int execute_packet_process(uint8_t identifier, uint8_t* payloadBuffer, uint8_t payloadLength) {
    int processResult = 0;
    bool handled = false;
    for (uint16_t i = 0; i < NUM_PROCESSES; i++) {
        if (processes[i]->identifier == identifier) {
            handled = true;
            processResult = processes[i]->process_packet(payloadBuffer, payloadLength);
        }

        if (debug) {
            printf("Executing process: %s, result: 0x%x\n", processes[i]->name, processResult);
        }
    }

    if (!handled) {
        return -1;
    }

    return processResult;
}