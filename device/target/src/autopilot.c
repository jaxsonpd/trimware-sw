#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


#include "avr_extends/GPIO.h"
#include "avr_extends/delay.h"
#include "avr_extends/UART.h"

#include "./autopilot/pin.h"

#include "./common/TM1638.h"


// The left display
struct TM1638Device dispL= {
    .name = "D Left",
};

// The right display
struct TM1638Device dispR = {
    .name = "D Right",
};

pin_t pin13;

bool init(void) {
    pin13 = PIN(PORTD, 6);
    GPIO_pin_init(pin13, OUTPUT);
    if (tm1638_init(&dispL, DISP_L_SELECT, DISP_DATA, DISP_CLK) != 0) {
        return 1;
    }

    if (tm1638_init(&dispR, DISP_R_SELECT, DISP_DATA, DISP_CLK) != 0) {
        return 2;
    }

    tm1638_reset(&dispL);
    tm1638_reset(&dispR);

    return true;
}

int main(void) {
    init();
    uint8_t values[6] = {10, 11, 12, 13, 14, 15};
    tm1638_write_string(&dispL, "FLC   ");
    tm1638_write(&dispR, 0x123456, "%x");

    while (true) {
        GPIO_set_output(pin13, true);
        delay_s(1);
        GPIO_set_output(pin13, false);
        delay_s(1);
    }

    return 0;
}