/** 
 * @file display_handler.h
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2025-03-22
 * @brief Controller for the display frequency module.
 */


#ifndef DISPLAY_HANDLER_H
#define DISPLAY_HANDLER_H


#include <stdint.h>
#include <stdbool.h>

/** 
 * @brief Initialize the display frequency module
 * 
 * @return 0 on success
 */
int display_handler_init(void);

/** 
 * @brief Update the display frequency module
 * 
 * @return 0 on success
 */
int display_handler_update(void);


#endif // DISPLAY_HANDLER_H