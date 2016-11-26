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
 *          You should only add or change files in the src directory!         *
 *----------------------------------------------------------------------------*/

#ifndef MEMORY_H_
#define MEMORY_H_

#include <stdbool.h>            // Boolean type and definitions
#include <stdint.h>             // Fixed-size integral types

/*----------------------------------------------------------------------------
 * Definitions
 *----------------------------------------------------------------------------*/

// The number of memory segments in the processor
#define NUM_MEM_REGIONS         5

// Forward declaration of the CPU state struct
struct cpu_state;

// The representation of a region/segment in memory
typedef struct mem_region {
    uint32_t base_addr;         // Base address of the memory region
    uint32_t max_size;          // Maximum permitted size for the memory region
    uint32_t size;              // Size of the memory region in bytes
    uint8_t *mem;               // Actual memory buffer for the region
    const char *hex_extension;  // File extension for the hex file the region
} mem_region_t;

// The representation for all the memory in the processor
typedef struct memory {
    int num_mem_regions;                        // Number of memory regions
    mem_region_t mem_regions[NUM_MEM_REGIONS];  // Memory regions in the CPU
} memory_t;

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
 * mem_load_program
 *
 * Initializes the memory subsystem part of the CPU state. This loads the memory
 * regions from the specified program into the CPU memory, and initializes them
 * to the values specified in the respective hex files. Program path can either
 * be a relative or absolute path to the program file.
 **/
int mem_load_program(struct cpu_state *cpu_state, const char *program_path);

/**
 * mem_unload_program
 *
 * Unloads a program previously loaded by mem_load_program. This cleans up and
 * frees the allocated memory for the processor's memory region.
 **/
void mem_unload_program(struct cpu_state *cpu_state);

/**
 * mem_range_valid
 *
 * Checks if the given memory range from start to end (inclusive) is valid.
 * Namely, this means that all addresses between start and end are valid.
 **/
bool mem_range_valid(const struct cpu_state *cpu_state, uint32_t start_addr,
        uint32_t end_addr);

/**
 * mem_find_address
 *
 * Find the address on the host machine that corresponds to the address in the
 * simulator. If no such address exists, return NULL.
 **/
uint8_t *mem_find_address(const struct cpu_state *cpu_state, uint32_t addr);

/**
 * mem_write_word
 *
 * Writes the specified word value to the given memory location. The given
 * address must be a valid memory location in the processor.
 **/
void mem_write_word(uint8_t *mem_addr, uint32_t value);

#endif /* MEMORY_H_ */
