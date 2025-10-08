/** 
 * @file pin.h
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2025-03-19
 * @brief Decliration for the pin usage for the device
 */


#ifndef PIN_H
#define PIN_H


#include <stdint.h>
#include <stdbool.h>

#include "avr_extends/GPIO.h"

// Frequency input pins
#define FINE_BUTTON_PIN_NUM 5
#define FINE_CHA_PIN_NUM 6
#define FINE_CHB_PIN_NUM 7

#define COARSE_BUTTON_PIN_NUM 2
#define COARSE_CHA_PIN_NUM 3
#define COARSE_CHB_PIN_NUM 4

#define BUTTON_DOWN_VALUE false

#define FINE_BUTTON_PIN PIN(PORTD, FINE_BUTTON_PIN_NUM)
#define FINE_CHA_PIN PIN(PORTD, FINE_CHA_PIN_NUM)
#define FINE_CHB_PIN PIN(PORTD, FINE_CHB_PIN_NUM)

#define COARSE_BUTTON_PIN PIN(PORTD, COARSE_BUTTON_PIN_NUM)
#define COARSE_CHA_PIN PIN(PORTD, COARSE_CHA_PIN_NUM)
#define COARSE_CHB_PIN PIN(PORTD, COARSE_CHB_PIN_NUM)

// Device select pins
#define DEVICE_SELECT_0_PIN PIN(PORTB, 0)
#define DEVICE_SELECT_1_PIN PIN(PORTB, 1)
#define DEVICE_SELECT_2_PIN PIN(PORTB, 2)

// Display pins
#define DISP_2_SELECT PIN(PORTC, 0)
#define DISP_1_SELECT PIN(PORTC, 1)
#define DISP_CLK PIN(PORTC, 2)
#define DISP_DATA PIN(PORTC, 3)


#endif // PIN_H