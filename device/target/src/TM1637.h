/** 
 * @file TM1637.h
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2025-01-02
 * @brief Declirations for the TM1637 8 digit display module driver
 */


#ifndef TM1637_H
#define TM1637_H


#include <stdint.h>
#include <stdbool.h>

#include "avr_extends/GPIO.h"   

struct TM1637Device {
    pin_t dataPin; // The data signal pin
    pin_t clockPin; // The clock signal pin
    char* name;
};

/** 
 * @brief Initialise a new driver
 * @param device The device struct to initialise
 * 
 * @return 0 if successful
 */
int tm1637_init(struct TM1637Device device);

/** 
 * @brief Write to the display a value
 * @param device the device struct to write to
 * @param value the value to write where each hex digit is the displayed value
 * 
 * @return 0 if successful
 */
int tm1637_write(struct TM1637Device device, uint32_t value);




#endif // TM1637_H