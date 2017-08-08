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

// Standard Includes
#include <string.h>         // strrchr function
#include <stdint.h>         // Fixed-size integral types

/*----------------------------------------------------------------------------
 * Miscellaneous Macros
 *----------------------------------------------------------------------------*/

/**
 * A macro representing only the basename of the full __FILE__ path.
 *
 * This is simply the filename with the extension, stripped of all parent
 * directories.
 **/
#define __FILENAME__ \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

/**
 * Gets the maximum of two values, which should be of the same type.
 **/
#define max(x, y) \
    (((x) > (y)) ? (x) : (y))

/**
 * Gets the minimum of two values, which should be of the same type.
 **/
#define min(x, y) \
    (((x) < (y)) ? (x) : (y))

/**
 * Gets the length of a statically allocated array.
 *
 * The result will be incorrect if array is not statically allocated (e.g. it is
 * a pointer).
 **/
#define array_len(array) \
    (sizeof(array) / sizeof((array)[0]))

/**
 * Gets the length of a statically allocated string.
 *
 * The result will be incorrect if the string is not statically allocated (e.g.
 * it is a pointer).
 **/
#define string_len(string) \
    (array_len(string) - 1)

/*----------------------------------------------------------------------------
 * Parsing Utilities
 *----------------------------------------------------------------------------*/

/**
 * Attempts to parse the given string as a signed decimal integer.
 *
 * If successful, the value pointer is updated with the integer value of the
 * string. Otherwise, a negative error code is returned, and the value of the
 * value pointer is not set.
 **/
int parse_int(const char *string, int *val);

/**
 * Attempts to parse the given string as a 32-bit unsigned hexadecimal integer.
 *
 * If successful, the value pointer is updated with the integer value of the
 * string. Otherwise, a negative error code is returned, and the value of the
 * value pointer is not set.
 **/
int parse_uint32_hex(const char *string, uint32_t *val);

/**
 * Attempts to parse the given string as a 32-bit integral value.
 *
 * The string can be either a signed decimal integer or signed or unsigned
 * hexadecimal integer. If successful, the value pointer is updated with the
 * integer value of the string. Otherwise, a negative error code is returned,
 * and the value of the value pointer is not set.
 **/
int parse_int32(const char *string, int32_t *val);

/*----------------------------------------------------------------------------
 * Bit Manipulation Utilities
 *----------------------------------------------------------------------------*/

/**
 * Gets the specified byte from the specified value.
 *
 * Byte must be a valid byte index within a 32-bit integer, which means it must
 * be between 0 and 3.
 **/
uint8_t get_byte(uint32_t value, int byte);

/**
 * Creates a new 32-bit value where the specified value is the specified byte of
 * the 32-bit value, and all other bytes in the word are 0.
 *
 * Byte must be a valid byte index within a 32-bit integer, which means it must
 * be between 0 and 3.
 **/
uint32_t set_byte(uint8_t value, int byte);

/*----------------------------------------------------------------------------
 * Libc Wrappers
 *----------------------------------------------------------------------------*/

/**
 * A wrapper around libc's snprintf function that provides sanity checks.
 *
 * This verifies that all of arguments were able to be formatted into the string
 * and checks for errors. If this is not the case, an error message is printed
 * out, and the program exits.
 **/
#define Snprintf(str, size, format, ...) \
    snprintf_wrapper(__FILENAME__, __LINE__, __func__, str, size, format, \
            ## __VA_ARGS__)

/**
 * A wrapper around libc's snprintf that provides sanity checks.
 *
 * The wrapper checks for errors and verifies that all arguments were able to be
 * formatted into the string. If either of these are not the case, then it
 * prints out an error message with the supplied call site information, and
 * exits.
 **/
void snprintf_wrapper(const char *filename, int line, const char *function_name,
        char *str, size_t size, const char *format, ...);

#endif /* LIBC_EXTENSIONS_H_ */
