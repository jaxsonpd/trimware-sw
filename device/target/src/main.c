
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

#include "custom_can_protocol/packet_handler.h"

#include "freq_info.h"
#include "channel_select.h"

bool debug = true;

pin_t pin13;

/**
 * @brief Read a packet from stdin
 * @param buffer a pointer to where to store the data
 *
 * @return the length of the packet parsed
 */

// uint16_t getPacket(uint8_t* buffer) {
//     uint16_t bufferIndex = 0;

//     int byte;
//     bool packetStarted = false;

//     while ((UART_data_available() || packetStarted) && (byte = getchar()) != EOF) {
//         uint8_t currentByte = (uint8_t)byte;

//         if (currentByte == PACKET_START_BYTE && !packetStarted) {
//             packetStarted = true;
//             bufferIndex = 0;
//             buffer[bufferIndex++] = currentByte;
//             continue;
//         }

//         if (packetStarted) {
//             buffer[bufferIndex++] = currentByte;

//             if (currentByte == PACKET_END_BYTE) {
//                 return bufferIndex;
//             }
//         }
//     }

//     return 0;
// }

// void print_packet(uint8_t buffer[]) {
//     uint16_t i = 0;
//     do {
//         printf("0x%x ", buffer[i]);
//     } while(buffer[i++] != PACKET_END_BYTE || i == 1);
// }

void setup(void) {
    pin13 = PIN(PORTB, 5);
    GPIO_pin_init(pin13, OUTPUT);

    UART_init_stdio(115200);
    printf("Radio: 1\n");
    delay_ms(1000);

    freq_info_init();
    channel_select_init();
}

/** 
 * @brief Assemble a frequency packet
 * @param buffer to store the assembled packet
 * @param standbyFreq the standby frequency
 * @param activeFreq the active frequency
 * 
 * @return the length of the assembled packet
 */
uint8_t assemble_freq_payload(uint8_t *buffer, freq_t standbyFreq, freq_t activeFreq) {
    uint8_t bufferIndex = 0;

    buffer[bufferIndex++] = (standbyFreq.freqMHz >> 8) & 0xFF;
    buffer[bufferIndex++] = standbyFreq.freqMHz & 0xFF;
    buffer[bufferIndex++] = (standbyFreq.freqKHz >> 8) & 0xFF;
    buffer[bufferIndex++] = standbyFreq.freqKHz & 0xFF;
    buffer[bufferIndex++] = (activeFreq.freqMHz >> 8) & 0xFF;
    buffer[bufferIndex++] = activeFreq.freqMHz & 0xFF;
    buffer[bufferIndex++] = (activeFreq.freqKHz >> 8) & 0xFF;
    buffer[bufferIndex++] = activeFreq.freqKHz & 0xFF;

    return bufferIndex;
}

int main(void) {
    setup();
    sei();

    freq_t standbyFreq_prev = freq_info_get(STANDBY_FREQ);
    freq_t activeFreq_prev = freq_info_get(ACTIVE_FREQ);
    uint8_t selectedChannel_prev = channel_select_get();

    while (true) {
        freq_t standbyFreq = freq_info_get(STANDBY_FREQ);
        freq_t activeFreq = freq_info_get(ACTIVE_FREQ);
        freq_info_check_swap();

        printf("Standby Freq: %u.%03u, Active Freq: %u.%03u\n", standbyFreq.freqMHz
           , standbyFreq.freqKHz, activeFreq.freqMHz, activeFreq.freqKHz);

        if (standbyFreq_prev.freqKHz != standbyFreq.freqKHz || 
            standbyFreq_prev.freqMHz != standbyFreq.freqMHz ||
            activeFreq_prev.freqKHz != activeFreq.freqKHz || 
            activeFreq_prev.freqMHz != activeFreq.freqMHz) {
            uint8_t payloadBuf[128] = {0};
            uint8_t payloadSize = assemble_freq_payload(payloadBuf, standbyFreq, activeFreq);
            
            // packet_send(putchar, payloadBuf, payloadSize, 0x01);
            standbyFreq_prev = standbyFreq;
            activeFreq_prev = activeFreq;
        }

        uint8_t selectedChannel = channel_select_get();
        if (selectedChannel_prev != selectedChannel) {
            uint8_t payloadBuf[1] = {selectedChannel};
            packet_send(putchar, payloadBuf, 0x01, 0x04);

            selectedChannel_prev = selectedChannel;
        }
        
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
