/** 
 * @file TM1638.h
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2025-03-19
 * @brief Header for the TM1638 display driver
 */


#ifndef TM1638_H
#define TM1638_H

#include <stdint.h>
#include <stdbool.h>

#include "avr_extends/GPIO.h"

struct TM1638Device {
    pin_t dataPin; // The data signal pin
    pin_t clockPin; // The clock signal pin
    pin_t stbPin; // The stb signal pin
    char* name;
};

/** 
 * @brief Initialise a new driver
 * @param device The device struct to initialise
 * 
 * @return 0 if successful
 */
int tm1638_init(struct TM1638Device device);

/** 
 * @brief Write to the display a value
 * @param device the device struct to write to
 * @param value the value to write where each hex digit is the displayed value
 * 
 * @return 0 if successful
 */
int tm1638_write(struct TM1638Device device, uint32_t value);


#endif // TM1638_H
