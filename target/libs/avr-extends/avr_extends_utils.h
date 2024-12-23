/** 
 * @file avr_extends_utils.h
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2024-10-17
 * @brief Varius utility functions for the library
 */


#ifndef AVR_EXTENDS_UTILS_H
#define AVR_EXTENDS_UTILS_H


#include <stdint.h>
#include <stdbool.h>

#ifdef __GNUC__
/**
 * @brief This macro is used to tell gcc that the function parameter is unused.
 * 
 * This can be used like so: void foo(int UNUSED(bar));
 * This will only work on gcc and clang.
 * 
 * @param x the attribute that is unused
 */
#  define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#  define UNUSED(x) UNUSED_ ## x
#endif

#ifdef __GNUC__
/**
 * @brief This macro is used to tell gcc that the function is unused.
 * 
 * This can be used like so: void UNUSED_FUNCTION(foo)(int UNUSED(bar));
 * This will only work on gcc and clang.
 * 
 * @param x the function that is unused
 */
#  define UNUSED_FUNCTION(x) __attribute__((__unused__)) UNUSED_ ## x
#else
#  define UNUSED_FUNCTION(x) UNUSED_ ## x
#endif

#endif // AVR_EXTENDS_UTILS_H