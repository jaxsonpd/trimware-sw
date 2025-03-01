
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

#include <avr/interrupt.h>

#include "freq_info.h"
#include "channel_select.h"

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

uint16_t getPacket(uint8_t* buffer) {
    uint16_t bufferIndex = 0;

    int byte;
    bool packetStarted = false;

    while ((UART_data_available() || packetStarted) && (byte = getchar()) != EOF) {
        uint8_t currentByte = (uint8_t)byte;

        if (currentByte == PACKET_START_BYTE && !packetStarted) {
            packetStarted = true;
            bufferIndex = 0;
            buffer[bufferIndex++] = currentByte;
            continue;
        }

        if (packetStarted) {
            buffer[bufferIndex++] = currentByte;

            if (currentByte == PACKET_END_BYTE) {
                return bufferIndex;
            }
        }
    }

    return 0;
}

void setup(void) {
    pin13 = PIN(PORTB, 5);
    GPIO_pin_init(pin13, OUTPUT);

    UART_init_stdio(115200);
    printf("UART up and running\n");
    delay_ms(1000);

    // init_processes();
}

void print_packet(uint8_t buffer[]) {
    uint16_t i = 0;
    do {
        printf("0x%x ", buffer[i]);
    } while(buffer[i++] != PACKET_END_BYTE || i == 1);

}

int main(void) {
    setup();
    sei();

    freq_info_init();
    channel_select_init();

    while (true) {
        freq_t standbyFreq = freq_info_get(STANDBY_FREQ);
        freq_t activeFreq = freq_info_get(ACTIVE_FREQ);
        freq_info_check_swap();
        uint8_t selectedValue = channel_select_get();
        printf("Selected type: %x, Selected standby: %u.%03u, Selected active: %u.%03u\n", selectedValue, 
            standbyFreq.freqMHz, standbyFreq.freqKHz, activeFreq.freqMHz, activeFreq.freqKHz);
        delay_ms(100);
    }
    

    // while (true)
    // {
    //     GPIO_pin_init(PIN(PORTB, 2), INPUT_NO_PULLUP);
    //     delay_ms(250);
    //     printf("Result: 0x%x", GPIO_get_state(PIN(PORTB, 2)));
    //     delay_ms(500);
    // }

    // // Storage for input from UART
    // uint8_t inputBuffer[1024] = { 0 };
    // uint16_t inputLength = 0;

    // // Flags
    // bool newPacket = false;

    // // Results
    // packetStatus_t validationStatus = PACKET_VALID;

    // while (true) {
    //     // Get packet from uart
    //     if (UART_data_available()) {
    //         inputLength = getPacket(inputBuffer);
    //         newPacket = inputLength > 0;
            
    //         if (debug) {
    //             printf("Packet Length 0x%x ", inputLength);
    //             if (newPacket) {
    //                 print_packet(inputBuffer);
    //             }
    //             printf("\n");
    //         }
    //     }

    //     // Validate packet
    //     if (newPacket) {
    //         validationStatus = packet_validate(inputBuffer, inputLength);

    //         if (validationStatus != PACKET_VALID) {
    //             if (debug) {
    //                 printf("ERROR: Last Packet invalid status: 0x%x, packet: %s\n", validationStatus, inputBuffer);
    //             }
    //         }
    //     }

    //     // Use packet to update commands
    //     if (newPacket && validationStatus == PACKET_VALID) {
    //         execute_packet_process(inputBuffer[PACKET_IDENTIFIER_LOC],
    //             &inputBuffer[PACKET_PAYLOAD_START_LOC],
    //             inputBuffer[PACKET_LENGTH_LOC]);
    //     }

    //     for (uint16_t i = 0; i < 1024; i++) {
    //         inputBuffer[i] = 0;
    //     }
    //     newPacket = false;

    //     update_processes();

    //     GPIO_toggle_output(pin13);
    //     // delay_ms(500);
    // }
    // return 0;
}
