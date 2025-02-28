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

#include "channel_select.h"

#define CHANNEL_SELECT_0_PIN PIN(PORTB, 0)
#define CHANNEL_SELECT_1_PIN PIN(PORTB, 1)
#define CHANNEL_SELECT_2_PIN PIN(PORTB, 2)

int channel_select_init(void) {
    GPIO_pin_init(CHANNEL_SELECT_0_PIN, INPUT_NO_PULLUP);
    GPIO_pin_init(CHANNEL_SELECT_1_PIN, INPUT_NO_PULLUP);
    GPIO_pin_init(CHANNEL_SELECT_2_PIN, INPUT_NO_PULLUP);

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