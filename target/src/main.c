
#include "avr_extends/GPIO.h"


int main(void)
{
    pin_t pin13 = PIN(PORTB, 5);

    GPIO_pin_init(pin13, OUTPUT);

    while(true) {
        GPIO_set_output(pin13, true);
    }
    return 0;
}
