/** 
 * @file channel_select.h
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2025-02-28
 * @brief Module to handle channel selection
 */


#ifndef CHANNEL_SELECT_H
#define CHANNEL_SELECT_H


#include <stdint.h>
#include <stdbool.h>

/** 
 * @brief Initialise the channel select value
 * 
 * @return 0 if successful
 */
int channel_select_init(void);

/** 
 * @brief Get the current selected channel
 * 
 * @return The channel selected from 0 - 7
 */
uint8_t channel_select_get(void);


#endif // CHANNEL_SELECT_H