/**
 * libc_extensions.h
 *
 * RISC-V 32-bit Instruction Level Simulator
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

// Standard Includes
#include <stdlib.h>             // Exit, strtol functions
#include <stdio.h>              // snprintf function
#include <stdarg.h>             // Variable argument list functions and list
#include <stdint.h>             // Fixed-size integral types

// Standard Includes
#include <assert.h>             // Assert macro
#include <string.h>             // Strerror function
#include <limits.h>             // Limits for integer types
#include <errno.h>              // Error codes and perror

// Local Includes
#include "libc_extensions.h"    // Our interface

/*----------------------------------------------------------------------------
 * Parsing Utilities
 *----------------------------------------------------------------------------*/

/**
 * Attempts to parse the given string as a integer with the given base. If
 * successful, the value pointer is updated with the string's value.
 **/
static int parse_long(const char *string, int base, long *val)
{
    // Set errno explicitly to 0, so we know if strtol set it
    errno = 0;

    // Parse the decimal string as a signed long
    char *end_str;
    *val = strtol(string, &end_str, base);

    // Check for possible errors when parsing the value
    if (errno != 0) {
        return -errno;
    } else if (*end_str != '\0') {
        return -EINVAL;
    } else {
        return 0;
    }
}

/**
 * Attempts to parse the given string as a signed decimal integer.
 *
 * If successful, the value pointer is updated with the integer value of the
 * string. Otherwise, a negative error code is returned, and the value of the
 * value pointer is not set.
 **/
int parse_int(const char *string, int *val)
{
    // Parse the value as a signed long, and check that it is in rage
    long parsed_val;
    int rc = parse_long(string, 10, &parsed_val);
    if (rc < 0) {
        return rc;
    } else if (parsed_val < INT_MIN || parsed_val > INT_MAX) {
        return -ERANGE;
    }

    // If we could parse the value, then cast it to an integer
    *val = (int)parsed_val;
    return 0;
}

/**
 * Attempts to parse the given string as a 32-bit unsigned hexadecimal integer.
 *
 * If successful, the value pointer is updated with the integer value of the
 * string. Otherwise, a negative error code is returned, and the value of the
 * value pointer is not set.
 **/
int parse_uint32_hex(const char *string, uint32_t *val)
{
    // Parse the value as a signed long, and check that it is in rage
    long parsed_val;
    int rc = parse_long(string, 16, &parsed_val);
    if (rc < 0) {
        return rc;
    } else if ((unsigned long)parsed_val > UINT32_MAX) {
        return -ERANGE;
    }

    // If we could parse the value, then cast it to an integer
    *val = (uint32_t)(int32_t)parsed_val;
    return 0;
}

/**
 * Attempts to parse the given string as a 32-bit integral value.
 *
 * The string can be either a signed decimal integer or signed or unsigned
 * hexadecimal integer. If successful, the value pointer is updated with the
 * integer value of the string. Otherwise, a negative error code is returned,
 * and the value of the value pointer is not set.
 **/
int parse_int32(const char *string, int32_t *val)
{
    /* First, try to parse the value as an integer value, and check that the
     * value is in range. */
    long parsed_val;
    int rc = parse_long(string, 10, &parsed_val);
    if (rc == 0 && (parsed_val < INT32_MIN || parsed_val > INT32_MAX)) {
        return -ERANGE;
    } else if (rc == 0) {
        *val = (int32_t)parsed_val;
    }

    // Otherwise, if we couldn't parse the value, try as a hexadecimal value
    rc = parse_long(string, 0, &parsed_val);
    if (rc < 0) {
        return rc;
    } else if (parsed_val < 0 && (unsigned long)-parsed_val > UINT32_MAX) {
        return -ERANGE;
    } else if (parsed_val > 0 && (unsigned long)parsed_val > UINT32_MAX) {
        return -ERANGE;
    }

    // If we could parse the value, then cast it to an integer
    *val = (int32_t)parsed_val;
    return 0;
}

/*----------------------------------------------------------------------------
 * Bit Manipulation Utilities
 *----------------------------------------------------------------------------*/

/**
 * Gets the specified byte from the specified value.
 *
 * Byte must be a valid byte index within a 32-bit integer, which means it must
 * be between 0 and 3.
 **/
uint8_t get_byte(uint32_t value, int byte)
{
    assert(0 <= byte && byte < (int)sizeof(value));

    return (value >> (8 * byte)) & 0xFF;
}

/**
 * Creates a new 32-bit value where the specified value is the specified byte of
 * the 32-bit value, and all other bytes in the word are 0.
 *
 * Byte must be a valid byte index within a 32-bit integer, which means it must
 * be between 0 and 3.
 **/
uint32_t set_byte(uint8_t value, int byte)
{
    assert(0 <= byte && byte < (int)sizeof(uint32_t));

    return ((uint32_t)value) << (8 * byte);
}

/*----------------------------------------------------------------------------
 * Libc Wrappers
 *----------------------------------------------------------------------------*/

/**
 * A wrapper around libc's snprintf that provides sanity checks.
 *
 * The wrapper checks for errors and verifies that all arguments were able to be
 * formatted into the string. If either of these are not the case, then it
 * prints out an error message with the supplied call site information, and
 * exits.
 **/
void snprintf_wrapper(const char *filename, int line, const char *function_name,
        char *str, size_t size, const char *format, ...)
{
    // Handle the variable argument list, and invoke snprintf with the args
    va_list args;
    va_start(args, format);
    ssize_t bytes_written = vsnprintf(str, size, format, args);
    va_end(args);

    // Check that call was successful, and the string could fit all the args
    if (bytes_written < 0) {
        int rc = errno;
        fprintf(stderr, "Error: %s: %s: %d: Unable to format string: %s\n",
                filename, function_name, line, strerror(errno));
        exit(rc);
    } else if (bytes_written >= (ssize_t)size) {
        fprintf(stderr, "Error: %s: %s: %d: String is too small to fit "
                "formatted arguments.\n", filename, function_name, line);
        exit(E2BIG);
    }

    return;
}
