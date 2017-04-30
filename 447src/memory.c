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
 *  - 2016 - 2017: Brandon Perez
 **/

/*----------------------------------------------------------------------------*
 *                          DO NOT MODIFY THIS FILE!                          *
 *          You should only add or change files in the src directory!         *
 *----------------------------------------------------------------------------*/

#include <stdlib.h>                 // Malloc and related functions
#include <stdio.h>                  // Printf and related functions
#include <stdint.h>                 // Fixed-size integral types

#include <assert.h>                 // Assert macro
#include <errno.h>                  // Error codes and perror
#include <string.h>                 // String manipulation functions and memset

#include "sim.h"                    // Interface to the core simulator
#include "riscv_abi.h"              // ABI registers and memory segments
#include "libc_extensions.h"        // Various utilities
#include "memory.h"                 // This file's interface to core simulator
#include "memory_shell.h"           // This file's itnerface to the shell

/*----------------------------------------------------------------------------
 * Internal Definitions
 *----------------------------------------------------------------------------*/

// The user text memory region, containing the user program
static const mem_region_t USER_TEXT_REGION = {
    .base_addr = USER_TEXT_START,
    .max_size = USER_DATA_START - USER_TEXT_START,
    .extension = ".text.bin",
};

// The user data memory region, containing user global variables
static const mem_region_t USER_DATA_REGION = {
    .base_addr = USER_DATA_START,
    .max_size = STACK_START - USER_DATA_START,
    .extension = ".data.bin"
};

// The stack memory region, containing local values in the program
static const mem_region_t STACK_REGION = {
    .base_addr = STACK_END - STACK_SIZE,
    .max_size = STACK_SIZE,
    .extension = NULL,
};

// The kernel text region, containing kernel code
static const mem_region_t KERNEL_TEXT_REGION = {
    .base_addr = KERNEL_TEXT_START,
    .max_size = KERNEL_DATA_START - KERNEL_TEXT_START,
    .extension = ".ktext.bin",
};

// The kernel data region, containing kernel global variables
static const mem_region_t KERNEL_DATA_REGION = {
    .base_addr = KERNEL_DATA_START,
    .max_size = UINT32_MAX - KERNEL_DATA_START,
    .extension = ".kdata.bin",
};

// An array of all the memory regions, and the number of memory regions
static const mem_region_t *const MEM_REGIONS[NUM_MEM_REGIONS] = {
    &USER_TEXT_REGION, &USER_DATA_REGION, &STACK_REGION, &KERNEL_TEXT_REGION,
    &KERNEL_DATA_REGION,
};

/*----------------------------------------------------------------------------
 * Shared Helper Functions
 *----------------------------------------------------------------------------*/

/**
 * mem_read_word
 *
 * Reads the specified value out from the given address in little endian order.
 * The address must be aligned on a 4-byte boundary
 **/
static uint32_t mem_read_word(const uint8_t *mem_addr)
{
    uint32_t value = 0;
    value |= set_byte(mem_addr[0], 0);
    value |= set_byte(mem_addr[1], 1);
    value |= set_byte(mem_addr[2], 2);
    value |= set_byte(mem_addr[3], 3);
    return value;
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
    uint8_t *mem_addr = mem_find_address(cpu_state, addr);
    if (mem_addr == NULL) {
        fprintf(stderr, "Encountered invalid memory address 0x%08x. Halting "
                "simulation.\n", addr);
        cpu_state->halted = true;
        return 0;
    }

    return mem_read_word(mem_addr);
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
    // Try to find the specified address
    uint8_t *mem_addr = mem_find_address(cpu_state, addr);
    if (mem_addr == NULL) {
        fprintf(stderr, "Encountered invalid memory address 0x%08x. Halting "
                "simulation.\n", addr);
        cpu_state->halted = true;
        return;
    }

    // Write the value out in little-endian order
    mem_write_word(mem_addr, value);
    return;
}

/*----------------------------------------------------------------------------
 * Shell Interface Functions
 *----------------------------------------------------------------------------*/

/**
 * malloc_mem_region
 *
 * Allocates the memory for the given region, which will have mem_region->size
 * bytes in it. Exits on error
 **/
static void malloc_mem_region(mem_region_t *mem_region)
{
    mem_region->mem = malloc(mem_region->size * sizeof(mem_region->mem[0]));
    if (mem_region->mem == NULL) {
        fprintf(stderr, "Error: Unable to allocate processor memory region.\n");
        exit(ENOMEM);
    }
    return;
}

/**
 * load_mem_region
 *
 * Loads the memory region from its corresponding data (binary) file. The size
 * of this file cannot exceed the max_size for the memory region.
 **/
static int load_mem_region(mem_region_t *mem_region, char *data_path)
{
    // Try to open the data file
    FILE *data_file = fopen(data_path, "r");
    if (data_file == NULL) {
        int rc = -errno;
        fprintf(stderr, "Error: %s: Unable to open file: %s.\n", data_path,
                strerror(errno));
        return rc;
    }

    // Determine the size of the memory region in the file.
    fseek(data_file, 0, SEEK_END);
    mem_region->size = ftell(data_file);
    rewind(data_file);
    if (mem_region->size == 0) {
        return 0;
    }

    // Allocate memory for the region only if the size does not exceed the max
    if (mem_region->size > mem_region->max_size) {
        fprintf(stderr, "Error: %s: File is too large for memory region.\n",
                data_path);
        fclose(data_file);
        return -EFBIG;
    } else if (mem_region->size % sizeof(uint32_t) != 0) {
        fprintf(stderr, "Error: %s: File size is not aligned to 4 bytes.\n.",
                data_path);
        fclose(data_file);
        return -EINVAL;
    }
    malloc_mem_region(mem_region);

    // Since the data file is binary, load it directly into memory
    int rc = 0;
    size_t bytes_read = fread(mem_region->mem, mem_region->size, 1, data_file);
    if (bytes_read != 1) {
        assert(ferror(data_file) || !feof(data_file));
        rc = -errno;
        fprintf(stderr, "Error: %s: Unable to read memory section file: %s.\n",
                data_path, strerror(errno));

        free(mem_region->mem);
        mem_region->mem = NULL;
    }

    // Close the data file
    fclose(data_file);
    return rc;
}

/**
 * join_strings
 *
 * Joins the two given strings into a new string. The new string is allocated by
 * malloc, and must be freed by the caller. Exits on error.
 **/
static char *join_strings(const char *string1, const char *string2)
{
    // Allocate a string that can hold the concatenation of the two strings
    size_t len = strlen(string1) + strlen(string2);
    char *string3 = malloc((len + 1) * sizeof(string3[0]));
    if (string3 == NULL) {
        fprintf(stderr, "Error: Unable to allocate buffer for new string.\n");
        exit(ENOMEM);
    }

    // Combine the two strings into the allocated string
    strcpy(string3, string1);
    strcat(string3, string2);
    return string3;
}

/**
 * mem_load_program
 *
 * Initializes the memory subsystem part of the CPU state. This loads the memory
 * regions from the specified program into the CPU memory, and initializes them
 * to the values specified in the respective data files. Program name should be
 * the path to the executable file (it has no extension).
 **/
int mem_load_program(cpu_state_t *cpu_state, const char *program_path)
{
    assert(array_len(cpu_state->memory.mem_regions) == array_len(MEM_REGIONS));

    // Set the number of memory regions, and zero out the mem regions array
    memory_t *memory = &cpu_state->memory;
    memory->num_mem_regions = array_len(MEM_REGIONS);
    memset(memory->mem_regions, 0, sizeof(memory->mem_regions));

    // Initialize each memory region, loading data from the associated data file
    int rc = 0;
    for (int i = 0; i < memory->num_mem_regions; i++)
    {
        // Copy the default values for the memory region in
        mem_region_t *mem_region = &memory->mem_regions[i];
        memcpy(mem_region, MEM_REGIONS[i], sizeof(*MEM_REGIONS[i]));

        /* If the memory region doesn't have a data file, we only allocate it.
         * In this case, the size of the memory region is max_size. */
        if (mem_region->extension == NULL) {
            mem_region->size = mem_region->max_size;
            malloc_mem_region(mem_region);
            continue;
        }

        /* Otherwise, combine the program path and extension to get the path
         * to the data file, load it, then free the buffer. */
        char *data_path = join_strings(program_path, mem_region->extension);
        rc = load_mem_region(mem_region, data_path);
        free(data_path);

        // Free the other memory regions if we failed to load this one
        if (rc < 0) {
            mem_unload_program(cpu_state);
            break;
        }
    }

    /* Point the PC to the user text segment, the stack pointer (x2) to the
     * stack segment, and the global pointer (x3) to the user data segment. */
    cpu_state->pc = USER_TEXT_START;
    cpu_state->regs[REG_SP] = STACK_END;
    cpu_state->regs[REG_GP] = USER_DATA_START;

    return rc;
}

/**
 * mem_unload_program
 *
 * Unloads a program previously loaded by mem_load_program. This cleans up and
 * frees the allocated memory for the processor's memory region.
 **/
void mem_unload_program(struct cpu_state *cpu_state)
{
    // Free each of the memory regions, if it has an allocated memory region
    memory_t *memory = &cpu_state->memory;
    for (int i = 0; i < memory->num_mem_regions; i++)
    {
        mem_region_t *mem_region = &memory->mem_regions[i];
        if (mem_region->mem != NULL) {
            free(mem_region->mem);
        }
    }

    return;
}

/**
 * mem_range_valid
 *
 * Checks if the given memory range from start to end (inclusive) is valid.
 * Namely, this means that all addresses between start and end are valid.
 **/
bool mem_range_valid(const cpu_state_t *cpu_state, uint32_t start_addr,
        uint32_t end_addr)
{
    assert(start_addr < end_addr);

    /* Iterate over the memory regions, checking if the range is completely
     * contained in any region. */
    for (int i = 0; i < cpu_state->memory.num_mem_regions; i++)
    {
        const mem_region_t *mem_region = &cpu_state->memory.mem_regions[i];
        uint32_t region_end_addr = mem_region->base_addr + mem_region->size;

        if (mem_region->base_addr <= start_addr && end_addr < region_end_addr) {
            return true;
        }
    }

    return false;
}

/**
 * mem_find_address
 *
 * Find the address on the host machine that corresponds to the address in the
 * simulator. If no such address exists, return NULL.
 **/
uint8_t *mem_find_address(const cpu_state_t *cpu_state, uint32_t addr)
{
    // Iterate over the memory regions, checking if the address lies in them
    for (int i = 0; i < cpu_state->memory.num_mem_regions; i++)
    {
        const mem_region_t *mem_region = &cpu_state->memory.mem_regions[i];
        uint32_t region_end_addr = mem_region->base_addr + mem_region->size;

        if (mem_region->base_addr <= addr && addr < region_end_addr) {
            uint32_t offset = addr - mem_region->base_addr;
            return &mem_region->mem[offset];
        }
    }

    return NULL;
}

/**
 * mem_write_word
 *
 * Writes the spcified value out to the given address in little endian order.
 * The address must be aligned on a 4-byte boundary.
 **/
void mem_write_word(uint8_t *mem_addr, uint32_t value)
{
    mem_addr[0] = get_byte(value, 0);
    mem_addr[1] = get_byte(value, 1);
    mem_addr[2] = get_byte(value, 2);
    mem_addr[3] = get_byte(value, 3);
    return;
}
