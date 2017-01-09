/**
 * libc_extensions.h
 *
 * RISC-V 32-bit Instruction Level Simulator.
 *
 * ECE 18-447
 * Carnegie Mellon University
 *
 * This file contains the interface to various helper functions and macros used
 * throughout the simulator.
 *
 * This is a general set of helper utilities and macros that can be considered
 * and extension to the standard C library.
 *
 * Authors:
 *  - 2016 - 2017: Brandon Perez
 **/

/*----------------------------------------------------------------------------*
 *                          DO NOT MODIFY THIS FILE!                          *
 *          You should only add or change files in the src directory!         *
 *----------------------------------------------------------------------------*/

#ifndef LIBC_EXTENSIONS_H_
#define LIBC_EXTENSIONS_H_

#include <string.h>         // strrchr function
#include <stdint.h>         // Fixed-size integral types

/*----------------------------------------------------------------------------
 * Miscellaneous Macros
 *----------------------------------------------------------------------------*/

/**
 * __FILENAME__
 *
 * A macro representing only the basename of the full __FILE__ path. This is
 * simply the filename with the extension, stripped of all parent directories.
 **/
#define __FILENAME__ \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

/**
 * max
 *
 * Gets the maximum of two values, which should be of the same type.
 **/
#define max(x, y) \
    (((x) > (y)) ? (x) : (y))

/**
 * array_len
 *
 * Gets the length of a statically allocated array. The result will be
 * incorrect if array is simply a pointer.
 **/
#define array_len(array) \
    (sizeof(array) / sizeof((array)[0]))

/**
 * string_len
 *
 * Gets the length of a statically allocated string. The result will be
 * incorrect if the string is simply a pointer.
 **/
#define string_len(string) \
    (array_len(string) - 1)

/*----------------------------------------------------------------------------
 * Parsing Utilities
 *----------------------------------------------------------------------------*/

/**
 * parse_int
 *
 * Attempts to parse the given string as a decimal integer. If successful, the
 * value pointer is updated with the integer value of the string.
 **/
int parse_int(const char *string, int *val);

/**
 * parse_uint32_hex
 *
 * Attempts to parse the given string as a 32-bit unsigned hexadecimal integer.
 * If successful, the value pointer is updated with the integer value of the
 * string.
 **/
int parse_uint32_hex(const char *string, uint32_t *val);

/**
 * parse_int32
 *
 * Attempts to parse the given string as a 32-bit value. The string can be
 * either a signed decimal integer or signed or unsigned hexadecimal integer.
 * If successful, the value pointer is updated with the integer value of the
 * string.
 **/
int parse_int32(const char *string, int32_t *val);

/*----------------------------------------------------------------------------
 * Bit Manipulation Utilities
 *----------------------------------------------------------------------------*/

/**
 * get_byte
 *
 * Gets the specified byte from the specified value, where the byte must be
 * between 0 and 3.
 **/
uint8_t get_byte(uint32_t value, int byte);

/**
 * set_byte
 *
 * Creates a new 32-bit value where the specified value is the specified byte of
 * the 32-bit value, and all other bytes in the word are 0.
 **/
uint32_t set_byte(uint8_t value, int byte);

/*----------------------------------------------------------------------------
 * Libc Wrappers
 *----------------------------------------------------------------------------*/

/**
 * Snprintf
 *
 * A wrapper around libc's snprintf function that verifies that all of arguments
 * were able to be formatted into the string and checks for errors. If this is
 * not the case, an error message is printed out, and the program exits.
 **/
#define Snprintf(str, size, format, ...) \
    snprintf_wrapper(__FILENAME__, __LINE__, __func__, str, size, format, \
            ## __VA_ARGS__)

/**
 * snprintf_wrapper
 *
 * The wrapper around snprintf invoked by the Snprintf macro. This should not be
 * called directly by the user. It simply takes information about where Snprintf
 * was called from to improve the error message.
 **/
void snprintf_wrapper(const char *filename, int line, const char *function_name,
        char *str, size_t size, const char *format, ...);

#endif /* LIBC_EXTENSIONS_H_ */
