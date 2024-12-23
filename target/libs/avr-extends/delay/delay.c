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
    volatile uint32_t count = (delay * F_CPU) / 6000UL; // IDK how this works 
    while (count--) {
        __asm__ __volatile__("nop");
    }
}

void delay_us(uint32_t delay) {
    volatile uint64_t count = (delay * F_CPU) / 19000000UL; // IDK how this works 
    while (count--) {
        __asm__ __volatile__("nop");
    }
    
}
