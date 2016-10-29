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

#include <stdio.h>          // Printf and related functions
#include <stdint.h>         // Fixed-size integral types

#include <assert.h>         // Assert macro

#include "sim.h"          // Interface to the core simulator
#include "memory.h"         // This file's interface

/*----------------------------------------------------------------------------
 * Internal Definitions
 *----------------------------------------------------------------------------*/

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

// The value memory is initalized to, for making detecting unitialized values
#define MEMORY_INIT_VALUE   0xCD

// Allocate memmory for the user text memory region
static uint8_t user_text_buffer[USER_TEXT_SIZE] = {MEMORY_INIT_VALUE};
static const mem_region_t user_text = {
    .base_addr = USER_TEXT_START,
    .size = sizeof(user_text_buffer),
    .mem = user_text_buffer,
};

// Allocate memory for the user data memory region
static uint8_t user_data_buffer[USER_DATA_SIZE] = {MEMORY_INIT_VALUE};
static const mem_region_t user_data = {
    .base_addr = USER_DATA_START,
    .size = sizeof(user_data_buffer),
    .mem = user_data_buffer,
};

// Allocate memory for the user stack memory region
static uint8_t user_stack_buffer[USER_STACK_SIZE] = {MEMORY_INIT_VALUE};
static const mem_region_t user_stack = {
    .base_addr = USER_STACK_START,
    .size = sizeof(user_stack_buffer),
    .mem = user_stack_buffer,
};

// Allocate memory for the kernel text memory region
static uint8_t kernel_text_buffer[KERNEL_TEXT_SIZE] = {MEMORY_INIT_VALUE};
static const mem_region_t kernel_text = {
    .base_addr = KERNEL_TEXT_START,
    .size = sizeof(kernel_text_buffer),
    .mem = kernel_text_buffer,
};

// Allocate memory for the kernel data memory region
static uint8_t kernel_data_buffer[KERNEL_DATA_SIZE] = {MEMORY_INIT_VALUE};
static const mem_region_t kernel_data = {
    .base_addr = KERNEL_DATA_START,
    .size = sizeof(kernel_data_buffer),
    .mem = kernel_data_buffer,
};

// Create a list of all the memory regions for the processor
static const mem_region_t *mem_regions[] = {
    &user_text,
    &user_data,
    &user_stack,
    &kernel_text,
    &kernel_data,
};
static const int num_mem_regions = sizeof(mem_regions) / sizeof(mem_regions[0]);

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
        const mem_region_t *mem_region = cpu_state->mem_regions[i];
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
 * Interface Functions
 *----------------------------------------------------------------------------*/

/**
 * mem_init
 *
 * Initializes the memory subsystem part of the CPU state. This only needs to be
 * called by the shell at startup time. The core simulator does not need this
 * function.
 **/
void mem_init(cpu_state_t *cpu_state)
{
    cpu_state->mem_regions = mem_regions;
    cpu_state->num_mem_regions = num_mem_regions;

    return;
}

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
