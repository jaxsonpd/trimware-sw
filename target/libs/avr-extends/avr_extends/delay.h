/** 
 * @file delay.h
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2024-12-23
 * @brief Declarations for a wrapper for the builtin delay functions of the avr lib
 * which are very limited. 
 * 
 * This module assumes that _delay_ms and _delay_us
 * can both perform a 1 unit wait.
 */


#ifndef DELAY_H
#define DELAY_H


#include <stdint.h>
#include <stdbool.h>

/** 
 * @brief Delay for a certain number of seconds
 * @param delay the number of s to wait
 * 
 */
void delay_s(uint32_t delay);

/** 
 * @brief Delay for a certain number of ms
 * @param delay the number of ms to wait
 * 
 */
void delay_ms(uint32_t delay);


/** 
 * @brief Delay for a certain number of micro seconds
 * @param delay the number of us to wait
 * 
 */
void delay_us(uint32_t delay);


#endif // DELAY_H