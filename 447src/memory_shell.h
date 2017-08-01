/**
 * memory.h
 *
 * RISC-V 32-bit Instruction Level Simulator.
 *
 * ECE 18-447
 * Carnegie Mellon University
 *
 * This file contains the interface to the memory backend for the simulator. The
 * backend handles abstracting the processor memory from the core simulator
 * functions.
 *
 * Specifically, this file contains the interface that is only used by the
 * shell. These functions should not be invoked by the core simulator.
 *
 * Authors:
 *  - 2016 - 2017: Brandon Perez
 **/

/*----------------------------------------------------------------------------*
 *                          DO NOT MODIFY THIS FILE!                          *
 *          You should only add or change files in the src directory!         *
 *----------------------------------------------------------------------------*/

#ifndef MEMORY_SHELL_H_
#define MEMORY_SHELL_H_

#include <stdbool.h>            // Boolean type and definitions
#include <stdint.h>             // Fixed-size integral types

/*----------------------------------------------------------------------------
 * Interface
 *----------------------------------------------------------------------------*/

/**
 * mem_load_program
 *
 * Initializes the memory subsystem part of the CPU state. This loads the memory
 * segments from the specified program into the CPU memory, and initializes them
 * to the values specified in the respective data files. Program path can either
 * be a relative or absolute path to the executable file (it has no extension).
 **/
int mem_load_program(cpu_state_t *cpu_state, const char *program_path);

/**
 * mem_unload_program
 *
 * Unloads a program previously loaded by mem_load_program. This cleans up and
 * frees the allocated memory for the processor's memory segments.
 **/
void mem_unload_program(cpu_state_t *cpu_state);

/**
 * mem_range_valid
 *
 * Checks if the given memory range from start to end (inclusive) is valid.
 * Namely, this means that all addresses between start and end are valid.
 **/
bool mem_range_valid(const cpu_state_t *cpu_state, uint32_t start_addr,
        uint32_t end_addr);

/**
 * mem_find_address
 *
 * Find the address on the host machine that corresponds to the address in the
 * simulator. If no such address exists, return NULL.
 **/
uint8_t *mem_find_address(const cpu_state_t *cpu_state, uint32_t addr);

/**
 * mem_write_word
 *
 * Writes the specified word value to the given memory location. The given
 * address must be a valid memory location in the processor.
 **/
void mem_write_word(uint8_t *mem_addr, uint32_t value);

#endif /* MEMORY_SHELL_H_ */
