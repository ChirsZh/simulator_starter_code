/**
 * memory.c
 *
 * RISC-V 32-bit Instruction Level Simulator.
 *
 * ECE 18-447
 * Carnegie Mellon University
 *
 * This file contains the memory backend for the simulator, which handles
 * abstracting the processor memory to the core simulator functions.
 *
 * Authors:
 *  - 2016: Brandon Perez
 **/

/*----------------------------------------------------------------------------*
 *                          DO NOT MODIFY THIS FILE!                          *
 *                 You should only add files or change sim.c!                 *
 *----------------------------------------------------------------------------*/

#include <stdlib.h>         // Malloc and related functions
#include <stdio.h>          // Printf and related functions
#include <stdint.h>         // Fixed-size integral types

#include <assert.h>         // Assert macro
#include <errno.h>          // Error codes and perror

#include "sim.h"            // Interface to the core simulator
#include "memory.h"         // This file's interface

/*----------------------------------------------------------------------------
 * Internal Definitions
 *----------------------------------------------------------------------------*/

// Macro to get the length of a statically allocated array
#define array_len(x) (sizeof(x) / sizeof(x[0]))

// The addresses and sizes of the user's stack, data, and text segments
#define USER_TEXT_START     0x00400000
#define USER_TEXT_SIZE      (1 * 1024 * 1024)
#define USER_DATA_START     0x10000000
#define USER_DATA_SIZE      (1 * 1024 * 1024)
#define USER_STACK_START    0x7ff00000
#define USER_STACK_SIZE     (1 * 1024 * 1024)

// The addresses and sizes of the kernel's stack, data, and text segments
#define KERNEL_DATA_START   0x90000000
#define KERNEL_DATA_SIZE    (1 * 1024 * 1024)
#define KERNEL_TEXT_START   0x80000000
#define KERNEL_TEXT_SIZE    (1 * 1024 * 1024)

// The starting addresses of each memory segment in the processor
static uint32_t MEM_REGION_STARTS[] = {
    USER_TEXT_START,
    USER_DATA_START,
    USER_STACK_START,
    KERNEL_TEXT_START,
    KERNEL_DATA_START,
};

// The sizes of each memory segment in the processor
static const uint32_t MEM_REGION_SIZES[] = {
    USER_TEXT_SIZE,
    USER_DATA_SIZE,
    USER_STACK_SIZE,
    KERNEL_TEXT_SIZE,
    KERNEL_DATA_SIZE,
};

// The number of memory segments in the processor
static const int NUM_MEM_REGIONS = array_len(MEM_REGION_STARTS);

/*----------------------------------------------------------------------------
 * Helper Functions
 *----------------------------------------------------------------------------*/

/**
 * find_address
 *
 * Find the address on the host machine that corresponds to the address in the
 * simulator. If no such address exists, return NULL.
 **/
static uint8_t *find_address(const cpu_state_t *cpu_state, uint32_t addr)
{
    // Iterate over the memory regions, checking if the address lies in them
    for (int i = 0; i < cpu_state->num_mem_regions; i++)
    {
        const mem_region_t *mem_region = &cpu_state->mem_regions[i];
        uint32_t region_end_addr = mem_region->base_addr + mem_region->size;

        if (mem_region->base_addr <= addr && addr < region_end_addr) {
            uint32_t offset = addr - mem_region->base_addr;
            return &mem_region->mem[offset];
        }
    }

    return NULL;
}

/**
 * get_byte
 *
 * Gets the specified byte from the specified value, where the byte must be
 * between 0 and 3.
 **/
static uint8_t get_byte(uint32_t value, int byte)
{
    assert(0 <= byte && byte < (int)sizeof(value));

    return (value >> (8 * byte)) & 0xFF;
}

/**
 * set_byte
 *
 * Creates a new 32-bit value where the specified value is the specified byte of
 * the 32-bit value, and all other bytes in the word are 0.
 **/
static uint32_t set_byte(uint8_t value, int byte)
{
    assert(0 <= byte && byte < (int)sizeof(uint32_t));

    return ((uint32_t)value) << (8 * byte);
}

/*----------------------------------------------------------------------------
 * Core Simulator Interface Functions
 *----------------------------------------------------------------------------*/

/**
 * mem_write32
 *
 * Reads the value at the specified address in the processor's memory. The
 * function ensures that the value is written in little-endian order. If the
 * address is unaligned or invalid, this function will stop the simulator on an
 * exception.
 **/
uint32_t mem_read32(cpu_state_t *cpu_state, uint32_t addr)
{
    // Try to find the specified address
    uint8_t *mem_addr = find_address(cpu_state, addr);
    if (mem_addr == NULL) {
        fprintf(stderr, "Encountered invalid memory address 0x%08x. Ending "
                "simulation.\n", addr);
        cpu_state->running = false;
        return 0;
    }

    // Read the value out in little-endian order
    uint32_t value = 0;
    value |= set_byte(mem_addr[0], 0);
    value |= set_byte(mem_addr[1], 1);
    value |= set_byte(mem_addr[2], 2);
    value |= set_byte(mem_addr[3], 3);

    return value;
}

/**
 * mem_write32
 *
 * Writes the specified value to the given address in the processor's memory.
 * The function ensures that the value is written in little-endian order. If the
 * address is unaligned or invalid, this function will stop the simulator on an
 * exception.
 **/
void mem_write32(cpu_state_t *cpu_state, uint32_t addr, uint32_t value)
{
    // TODO: Set run bit to 0
    // Try to find the specified address
    uint8_t *mem_addr = find_address(cpu_state, addr);
    if (mem_addr == NULL) {
        fprintf(stderr, "Encountered invalid memory address 0x%08x. Ending "
                "simulation.\n", addr);
        cpu_state->running = false;
        return;
    }

    // Write the value out in little-endian order
    mem_addr[0] = get_byte(value, 0);
    mem_addr[1] = get_byte(value, 1);
    mem_addr[2] = get_byte(value, 2);
    mem_addr[3] = get_byte(value, 3);

    return;
}

/*----------------------------------------------------------------------------
 * Shell Interface Functions
 *----------------------------------------------------------------------------*/

/**
 * mem_init
 *
 * Initializes the memory subsystem part of the CPU state. This allocates all of
 * the memory regions for the CPU, and intializes them to zero.
 **/
void mem_init(cpu_state_t *cpu_state)
{
    // Allocate the memory region metadata for the processor
    cpu_state->num_mem_regions = NUM_MEM_REGIONS;
    size_t region_size = sizeof(cpu_state->mem_regions[0]);
    cpu_state->mem_regions = calloc(cpu_state->num_mem_regions, region_size);
    if (cpu_state->mem_regions == NULL) {
        perror("Error: Unable to allocate memory regions structure.");
        exit(-errno);
    }

    // Initialize the memory regions
    for (int i = 0; i < cpu_state->num_mem_regions; i++)
    {
        // Setup the metadata for the memory region
        mem_region_t *mem_region = &cpu_state->mem_regions[i];
        mem_region->base_addr = MEM_REGION_STARTS[i];
        mem_region->size = MEM_REGION_SIZES[i];

        mem_region->mem = calloc(mem_region->size, sizeof(mem_region->mem[0]));
        if (mem_region->mem == NULL) {
            perror("Error: Unable to allocate memory regions for the program.");
            mem_destroy(cpu_state);
            exit(ENOMEM);
        }
    }

    return;
}

/**
 * mem_destroy
 *
 * Cleans up the data allocated by the memory part of the CPU state.
 **/
void mem_destroy(cpu_state_t *cpu_state)
{
    // Free each of the memory regions, if it has an allocated memory region
    for (int i = 0; i < cpu_state->num_mem_regions; i++)
    {
        mem_region_t *mem_region = &cpu_state->mem_regions[i];
        if (mem_region->mem != NULL) {
            free(mem_region->mem);
        }
    }

    // Free the memory region metadata structures
    free(cpu_state->mem_regions);
    return;
}

