/**
 * @file freq_display.c
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2025-01-02
 * @brief Implementation for the frequency display module
 */


#include <stdint.h>
#include <stdbool.h>

#include "freq_display.h"

#include "../common/TM1637.h"

struct TM1637Device activeDisplay = {
    .name = "Active Display"
};

struct TM1637Device standbyDisplay = {
    .name = "Standby Display"
};

int freq_display_init(void) {
    activeDisplay.clockPin = PIN(PORTB, 3); // Pin 11
    activeDisplay.dataPin = PIN(PORTB, 2); // Pin 10

    standbyDisplay.clockPin = PIN(PORTB, 1); // pin 9
    standbyDisplay.dataPin = PIN(PORTB, 0); // pin 8

    int result = 0;
    result |= tm1637_init(activeDisplay);
    result |= tm1637_init(standbyDisplay);

    return result;
}

int freq_display_write(freqOption_t type, freq_t frequency) {
    int result = 0;
    switch (type) {
    // case ACTIVE_FREQ:
    //     result = tm1637_write(activeDisplay, frequency);
    //     break;

    // case STANDBY_FREQ:
    //     result = tm1637_write(standbyDisplay, frequency);
    //     break;

    default:
        result = 1;
        break;
    }

    return result;
}