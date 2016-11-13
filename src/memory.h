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
 * Authors:
 *  - 2016: Brandon Perez
 **/

/*----------------------------------------------------------------------------*
 *                          DO NOT MODIFY THIS FILE!                          *
 *                 You should only add files or change sim.c!                 *
 *----------------------------------------------------------------------------*/

#ifndef MEMORY_H_
#define MEMORY_H_

#include <stdint.h>         // Fixed-size integral types

/*----------------------------------------------------------------------------
 * Definitions
 *----------------------------------------------------------------------------*/

// Forward declaration of the CPU state struct
struct cpu_state;

// The representation of a region/segment in memory
typedef struct mem_region {
    uint32_t base_addr;     // Base address of the memory region
    uint32_t size;          // Size of the memory region in bytes
    uint8_t *mem;           // Actual memory buffer for the region
} mem_region_t;

/*----------------------------------------------------------------------------
 * Interface to the Core Simulator
 *----------------------------------------------------------------------------*/

/**
 * mem_write32
 *
 * Reads the value at the specified address in the processor's memory. The
 * function ensures that the value is written in little-endian order. If the
 * address is unaligned or invalid, this function will stop the simulator on an
 * exception.
 **/
uint32_t mem_read32(struct cpu_state *cpu_state, uint32_t addr);

/**
 * mem_write32
 *
 * Writes the specified value to the given address in the processor's memory.
 * The function ensures that the value is written in little-endian order. If the
 * address is unaligned or invalid, this function will stop the simulator on an
 * exception.
 **/
void mem_write32(struct cpu_state *cpu_state, uint32_t addr, uint32_t value);

/*----------------------------------------------------------------------------
 * Interface to the Shell
 *----------------------------------------------------------------------------*/

/**
 * mem_init
 *
 * Initializes the memory subsystem part of the CPU state. This loads the memory
 * regions from the specified program into the CPU memory, and initializes them
 * to the values specified in the respective hex files. Program name should be
 * the path to the assembly file without the extension.
 **/
void mem_init(struct cpu_state *cpu_state, const char *program_path);

/**
 * mem_destroy
 *
 * Cleans up the data allocated by the memory part of the CPU state. This frees
 * the allocated memory for the processor's memory regions.
 **/
void mem_destroy(struct cpu_state *cpu_state);

#endif /* MEMORY_H_ */
