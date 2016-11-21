/**
 * parse.h
 *
 * RISC-V 32-bit Instruction Level Simulator
 *
 * ECE 18-447
 * Carnegie Mellon Univeristy
 *
 * This file conatins the interface to parsing utilities used throughout the
 * simulator.
 *
 * The parsing functions parses various integer types from files or from the
 * user input.
 *
 * Authors:
 *  - 2016: Brandon Perez
 **/

/*----------------------------------------------------------------------------*
 *                          DO NOT MODIFY THIS FILE!                          *
 *                 You should only add files or change sim.c!                 *
 *----------------------------------------------------------------------------*/

#ifndef PARSE_H_
#define PARSE_H_

#include <stdint.h>         // Fixed-size integral types

/**
 * parse_int
 *
 * Attempts to parse the given string as a decimal integer. If successful, the
 * value pointer is updated with the integer value of the string.
 **/
int parse_int(const char *string, int *val);

/**
 * parse_int32_hex
 *
 * Attempts to parse the given string as a 32-bit hexadecimal integer. If
 * successful, the value pointer is updated with the integer value of the
 * string. Negative signs are permitted in the string.
 **/
int parse_int32_hex(const char *string, int32_t *val);

/**
 * parse_int32
 *
 * Attempts to parse the given string as a 32-bit hexadecimal or decimal
 * integer. If successful, the value pointer is updated with the integer value
 * of the string. Negative signs are permitted in the string.
 **/
int parse_int32(const char *string, int32_t *val);

#endif /* PARSE_H_ */
