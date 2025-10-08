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

    // Private
    uint8_t _dots; // The dots to display 
    uint8_t _digits_enables; // The digit enables
    uint8_t _brightness; // The brightness of the display
};

/**
 * @brief Initialise a new driver
 * @param device The device struct to initialise
 * @param stbPin The stb pin
 * @param dataPin The data pin
 * @param clockPin The clock pin
 * 
 * @return 0 if successful
 */
int tm1638_init(struct TM1638Device* device, pin_t stbPin, pin_t dataPin, pin_t clockPin);

/**
 * @brief Enable a dot
 * @param device the device struct to manupulate
 * @param digit the digit to change
 * @param enable true to enable the dot
 *
 */
void tm1638_enable_dot(struct TM1638Device* device, uint8_t digit, bool enable);

/** 
 * @brief Enable digits to be displayed
 * @param device the struct to manupulate
 * @param digit the digit to change
 * @param enable true to enable the digit
 * 
 */
void tm1638_enable_digit(struct TM1638Device* device, uint8_t digit, bool enable);

/**
 * @brief Write to the display a value
 * @param device the device struct to write to
 * @param value the value to write where each hex digit is the displayed value
 * @param format the format to use ether "%x" or "%d"
 *
 * @return 0 if successful
 */
int tm1638_write(struct TM1638Device* device, uint32_t value, char* format);

/**
 * @brief Write digits to the display
 * @param device the device struct to use
 * @param startingDigit the starting digit to write to
 * @param values the values to write
 * @param valueLen the length of the values array
 *
 * @return 0 if successful
 */
int tm1638_write_digits(struct TM1638Device* device, uint8_t startingDigit, uint8_t values[], uint8_t valueLen);

/** 
 * @brief Clear the screen turning off all dots and on all digits
 * @param device the device struct to use
 * 
 */
void tm1638_reset(struct TM1638Device* device);

/** 
 * @brief Control whether the display is on or off
 * @param device the device struct to use
 * @param state true to turn on the display
 * 
 */
void tm1638_set_display_state(struct TM1638Device* device, bool state);

/** 
 * @brief Control the display brightness
 * @param device the device struct to use
 * @param brightness the brightness to set 0-7
 * 
 */
void tm1638_set_brightness(struct TM1638Device* device, uint8_t brightness);


#endif // TM1638_H
