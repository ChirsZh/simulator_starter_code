/**
 * parse.c
 *
 * RISC-V 32-bit Instruction Level Simulator
 *
 * ECE 18-447
 * Carnegie Mellon Univeristy
 *
 * This file contains miscellaneous utilities used throughout the simulator.
 *
 * The utilities consist of parsing helper functions for integers and other
 * values, and various functions to get information about files.
 *
 * Authors:
 *  - 2016: Brandon Perez
 **/

/*----------------------------------------------------------------------------*
 *                          DO NOT MODIFY THIS FILE!                          *
 *                 You should only add files or change sim.c!                 *
 *----------------------------------------------------------------------------*/

#include <stdlib.h>         // Malloc and related functions
#include <stdint.h>         // Fixed-size integral types

#include <limits.h>         // Limits for integer types
#include <errno.h>          // Error codes and perror

/*----------------------------------------------------------------------------
 * Parsing Utilities
 *----------------------------------------------------------------------------*/

/**
 * parse_long
 *
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
 * parse_int
 *
 * Attempts to parse the given string as a decimal integer. If successful, the
 * value pointer is updated with the integer value of the string.
 **/
int parse_int(const char *string, int *val)
{
    // Parse the value as a signed long, and check that it is in rage
    long parsed_val;
    int rc = parse_long(string, 10, &parsed_val);
    if (rc < 0) {
        return rc;
    } else if (parsed_val < (long)INT_MIN || parsed_val > (long)INT_MAX) {
        return -ERANGE;
    }

    // If we could parse the value, then cast it to an integer
    *val = (int)parsed_val;
    return 0;
}

/**
 * parse_int32_hex
 *
 * Attempts to parse the given string as a 32-bit hexadecimal integer. If
 * successful, the value pointer is updated with the integer value of the
 * string. Negative signs are permitted in the string.
 **/
int parse_int32_hex(const char *string, int32_t *val)
{
    // Parse the value as a signed long, and check that it is in rage
    long parsed_val;
    int rc = parse_long(string, 16, &parsed_val);
    if (rc < 0) {
        return rc;
    } else if (parsed_val < (long)INT32_MIN || parsed_val > (long)INT32_MAX) {
        return -ERANGE;
    }

    // If we could parse the value, then cast it to an integer
    *val = (int32_t)parsed_val;
    return 0;

}

/**
 * parse_int32
 *
 * Attempts to parse the given string as a 32-bit hexadecimal or decimal
 * integer. If successful, the value pointer is updated with the integer value
 * of the string. Negative signs are permitted in the string.
 **/
int parse_int32(const char *string, int32_t *val)
{
    // First, try to parse the value as a hexadecimal string
    int rc = parse_int32_hex(string, val);
    if (rc == 0) {
        return 0;
    }

    // Otherwise, try to parse the value a decimal string
    long parsed_val;
    rc = parse_long(string, 10, &parsed_val);
    if (rc < 0) {
        return rc;
    } else if (parsed_val < (long)INT32_MIN || parsed_val > (long)INT32_MAX) {
        return -ERANGE;
    }

    // If we could parse the value, then cast it to an integer
    *val = (int32_t)parsed_val;
    return 0;
}
