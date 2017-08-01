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
 * Specifically, this file contains the interface that is only used by the core
 * simulator, and definitions required to define the CPU state.
 *
 * Authors:
 *  - 2016 - 2017: Brandon Perez
 **/

/*----------------------------------------------------------------------------*
 *                          DO NOT MODIFY THIS FILE!                          *
 *          You should only add or change files in the src directory!         *
 *----------------------------------------------------------------------------*/

#ifndef MEMORY_H_
#define MEMORY_H_

#include <stdint.h>             // Fixed-size integral types

/*----------------------------------------------------------------------------
 * Definitions
 *----------------------------------------------------------------------------*/

// The number of memory segments in the processor
#define NUM_MEM_REGIONS         5

// Forward declaration of the CPU state struct
struct cpu_state;

// The representation of a segment in memory
typedef struct {
    uint32_t base_addr;         // Base address of the memory segment
    uint32_t max_size;          // Maximum permitted size for the memory segment
    uint32_t size;              // Size of the memory segment in bytes
    uint8_t *mem;               // Actual memory buffer for the segment
    const char *extension;      // File extension for the segment's data file
} mem_segment_t;

// The representation for all the memory in the processor
typedef struct memory {
    int num_segments;                         // Number of memory segments
    mem_segment_t segments[NUM_MEM_REGIONS];  // Memory segments in the CPU
} memory_t;

/*----------------------------------------------------------------------------
 * Interface
 *----------------------------------------------------------------------------*/

/**
 * mem_write32
 *
 * Reads the value at the specified address in the processor's memory. The
 * function ensures that the value is written in little-endian order. If the
 * address is invalid, this function will mark the CPU as halted.
 **/
uint32_t mem_read32(struct cpu_state *cpu_state, uint32_t addr);

/**
 * mem_write32
 *
 * Writes the specified value to the given address in the processor's memory.
 * The function ensures that the value is written in little-endian order. If the
 * address is invalid, this function will mark the CPU as halted.
 **/
void mem_write32(struct cpu_state *cpu_state, uint32_t addr, uint32_t value);

#endif /* MEMORY_H_ */
