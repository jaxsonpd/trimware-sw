/**
 * @file device_select.c
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2025-02-28
 * @brief Module to handle radio device selection
 */


#include <stdint.h>
#include <stdbool.h>

#include <stdio.h>

#include "avr_extends/GPIO.h"

#include "custom_can_protocol/packet_processing.h"
#include "custom_can_protocol/packet_handler.h"

#include "pin.h"

#include "device_select.h"

uint8_t selectedDevice_prev = 0;

int device_select_init(void) {
    GPIO_pin_init(DEVICE_SELECT_0_PIN, INPUT_NO_PULLUP);
    GPIO_pin_init(DEVICE_SELECT_1_PIN, INPUT_NO_PULLUP);
    GPIO_pin_init(DEVICE_SELECT_2_PIN, INPUT_NO_PULLUP);

    device_select_update();

    return 0;
}

uint8_t device_select_get(void) {
    return selectedDevice_prev;
}

uint16_t device_select_packet_assemble(uint8_t* buffer) {
    uint8_t bufferLength = 0;

    buffer[bufferLength++] = device_select_get();

    return bufferLength;
}



bool device_select_update(void) {
    bool select_0 = GPIO_get_state(DEVICE_SELECT_0_PIN);
    bool select_1 = GPIO_get_state(DEVICE_SELECT_1_PIN);
    bool select_2 = GPIO_get_state(DEVICE_SELECT_2_PIN);

    uint8_t value;
    if (select_0 && select_1 && select_2) {
        value = 0;
    } else {
        value = (select_0 << 2) + (select_1 << 1) + select_2;
    }

    bool result = selectedDevice_prev != value;
    selectedDevice_prev = value;

    return result;
}

packetProcessingResult_t device_select_packet_cb(uint8_t* payload, uint16_t payloadLen) {
    uint8_t payloadBuf[5] = { 0 };
    uint16_t payloadLenCalced = device_select_packet_assemble(payloadBuf);

    packet_send(putchar, payloadBuf, payloadLenCalced, 0x04);
    return PROCESS_COMPLETE;
}