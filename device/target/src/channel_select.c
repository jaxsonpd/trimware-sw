/** 
 * @file channel_select.c
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2025-02-28
 * @brief Module to handle channel selection
 */


#include <stdint.h>
#include <stdbool.h>

#include <stdio.h>

#include "avr_extends/GPIO.h"

#include "custom_can_protocol/packet_processing.h"
#include "custom_can_protocol/packet_handler.h"

#include "channel_select.h"

#define CHANNEL_SELECT_0_PIN PIN(PORTB, 0)
#define CHANNEL_SELECT_1_PIN PIN(PORTB, 1)
#define CHANNEL_SELECT_2_PIN PIN(PORTB, 2)

uint8_t selectedChannel_prev = 0;

int channel_select_init(void) {
    GPIO_pin_init(CHANNEL_SELECT_0_PIN, INPUT_NO_PULLUP);
    GPIO_pin_init(CHANNEL_SELECT_1_PIN, INPUT_NO_PULLUP);
    GPIO_pin_init(CHANNEL_SELECT_2_PIN, INPUT_NO_PULLUP);

    selectedChannel_prev = channel_select_get();

    return 0;
}

uint8_t channel_select_get(void) {
    bool select_0 = GPIO_get_state(CHANNEL_SELECT_0_PIN);
    bool select_1 = GPIO_get_state(CHANNEL_SELECT_1_PIN);
    bool select_2 = GPIO_get_state(CHANNEL_SELECT_2_PIN);
    
    uint8_t value;
    if (select_0 && select_1 && select_2) {
        value = 0;
    } else {
        value = (select_0 << 2) + (select_1 << 1) + select_2;
    }
        
    return value;
}

uint16_t channel_select_packet_assemble(uint8_t *buffer) {
    uint8_t bufferLength = 0;

    buffer[bufferLength++] = channel_select_get();

    selectedChannel_prev = channel_select_get();

    return bufferLength;
}



bool chanel_select_changed(void) {
    return selectedChannel_prev != channel_select_get();
}

packetProcessingResult_t channel_select_packet_cb(uint8_t *payload, uint16_t payloadLen) {
    uint8_t payloadBuf[5] = {0};
    uint16_t payloadLenCalced = channel_select_packet_assemble(payloadBuf);

    packet_send(putchar, payloadBuf, payloadLenCalced, 0x04);
    return PROCESS_COMPLETE;
}