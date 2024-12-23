/** 
 * @file delay.c
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2024-12-23
 * @brief Implementation for the avr delay wrappers
 */


#include <stdint.h>
#include <stdbool.h>

#include <util/delay.h>

#include "../avr_extends/delay.h"

void delay_s(uint32_t delay) {
    for (uint32_t i = 0; i < delay; i++)
    {
        delay_ms(1000);
    }
}

void delay_ms(uint32_t delay) {
    for (uint32_t i = 0; i < delay; i++) {
        _delay_ms(1);
    }
}

void delay_us(uint32_t delay) {
    for (uint32_t i = 0; i < delay; i++)
    {
        _delay_us(1);
    }
    
}
