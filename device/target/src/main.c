
/**
 * @file main.c
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2024-12-23
 * @brief Main file for the flight simulator radio panel project
 */


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>


#include "avr_extends/GPIO.h"
#include "avr_extends/delay.h"
#include "avr_extends/UART.h"

#include "packet_handler.h"
#include "process_handler.h"

#include "flags.h"

bool debug = true;

pin_t pin13;

/** 
 * @brief Read a packet from stdin
 * @param buffer a pointer to where to store the data
 * 
 * @return the length of the packet parsed
 */

size_t getPacket(uint8_t *buffer) {
    size_t bufferIndex = 0;

    int byte;
    bool packetStarted = false;

    while ((byte = getchar()) != EOF) {
        uint8_t currentByte = (uint8_t)byte;

        if (currentByte == PACKET_START_BYTE) {
            packetStarted = true;
            bufferIndex = 0;
            buffer[bufferIndex++] = currentByte;
            continue;
        }

        if (packetStarted) {
            buffer[bufferIndex++] = currentByte;

            if (currentByte == PACKET_END_BYTE) {
                // Found a complete packet
                buffer[bufferIndex] = '\0'; // Null-terminate the string
                return bufferIndex; // Return the size of the packet
            }
        }
    }

    return 0; // Return 0 if no packet is found
}

void setup(void) {
    pin13 = PIN(PORTB, 5);
    GPIO_pin_init(pin13, OUTPUT);

    UART_init_stdio(9600);
    printf("UART up and running");
}

int main(void) {
    
    setup();

    // Storage from input from UART
    uint8_t inputBuffer[1024] = {0};
    uint16_t inputLength = 0;

    // Flags
    bool newPacket = false;
    
    // Results
    packetStatus_t validationStatus = PACKET_VALID;

    while (true) {
        // Get packet from uart
        if (UART_data_available()) {
            inputLength = getPacket(inputBuffer);
            newPacket = inputLength > 0;
        }

        // Validate packet
        if (newPacket) {
            validationStatus = packet_validate(inputBuffer);

            if (debug && validationStatus != PACKET_VALID) {
                printf("ERROR: Last Packet invalid status: 0x%x\n", validationStatus);
            }
        }

        // Use packet to update commands
        if (newPacket && validationStatus == PACKET_VALID) {
            execute_packet_process(inputBuffer[PACKET_IDENTIFIER_LOC],
                                    inputBuffer[PACKET_PAYLOAD_START_LOC],
                                    inputBuffer[PACKET_LENGTH_LOC]);
        } 

        update_processes();

        GPIO_toggle_output(pin13);
        delay_ms(500);
    }
    return 0;
}
