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

// Standard Includes
#include <stdlib.h>                 // Malloc and related functions
#include <stdio.h>                  // Printf and related functions
#include <stdint.h>                 // Fixed-size integral types
#include <stdbool.h>                // Definition of the boolean type

// Standard Includes
#include <assert.h>                 // Assert macro
#include <errno.h>                  // Error codes and perror
#include <string.h>                 // String manipulation functions and memset

// 18-447 Simulator Includes
#include <sim.h>                    // Interface to the core simulator
#include <riscv_abi.h>              // ABI registers and memory segments
#include <register_file.h>          // Interface to the register file
#include <memory.h>                 // This file's interface to core simulator

// Local Includes
#include "libc_extensions.h"        // Various utilities
#include "memory_segments.h"        // Definition of memory segment constants
#include "memory_shell.h"           // This file's interface to the shell

/*----------------------------------------------------------------------------
 * Shared Helper Functions
 *----------------------------------------------------------------------------*/

/**
 * Reads the specified value out from the given address in the segment in
 * little-endian order.
 **/
static uint32_t mem_read_word(const mem_segment_t *segment, uint32_t addr)
{
    assert(segment->base_addr <= addr &&
            addr < segment->base_addr + segment->size);

    const uint8_t *mem_addr = &segment->mem[addr - segment->base_addr];
    uint32_t value = 0;
    for (int i = 0; i < (int)sizeof(uint32_t); i++)
    {
        value |= set_byte(mem_addr[i], i);
    }

    return value;
}

/*----------------------------------------------------------------------------
 * Core Simulator Interface Functions
 *----------------------------------------------------------------------------*/

/**
 * Reads the value at the specified address in the processor's memory.
 *
 * This function ensures that the value is read in little-endian order from the
 * address. If the address is invalid or it is not aligned to a 4-byte boundary,
 * then this function will mark the CPU as halted, and print out an error
 * message.
 *
 * Inputs:
 *  - cpu_state     The CPU state structure for the processor.
 *  - addr          The address from which to read the value.
 *
 * Outputs:
 *  - cpu_state     If the address is misaligned or invalid, the halted field
 *                  will be set to true.
 *  - return        The value at the given address in the CPU's memory.
 **/
uint32_t mem_read32(cpu_state_t *cpu_state, uint32_t addr)
{
    // Make sure the address is aligned
    if (addr % sizeof(uint32_t) != 0) {
        fprintf(stderr, "Encountered an unaligned memory address 0x%08x. "
                "Halting simulation.\n", addr);
        cpu_state->halted = true;
        return 0;
    }

    // Try to find the specified address
    mem_segment_t *segment = mem_find_segment(cpu_state, addr);
    if (segment == NULL) {
        fprintf(stderr, "Encountered invalid memory address 0x%08x. Halting "
                "simulation.\n", addr);
        cpu_state->halted = true;
        return 0;
    }

    return mem_read_word(segment, addr);
}

/**
 * Writes the specified value to the given address in the processor's memory.
 *
 * The function ensures that the value is written in little-endian order to the
 * address. If the address is invalid or it is not aligned to a 4-byte boundary,
 * then this function will mark the CPU as halted, and no update to memory
 * happens.
 *
 * Inputs:
 *  - cpu_state     The CPU state structure for the processor.
 *  - addr          The address to which to write the value.
 *  - value         The value to write to the given address.
 *
 * Outputs:
 *  - cpu_state     If the address is misaligned or invalid, the halted field
 *                  will be set to true. The processor memory is also
 *                  appropriately updated.
 **/
void mem_write32(cpu_state_t *cpu_state, uint32_t addr, uint32_t value)
{
    // Make sure the address is aligned
    if (addr % sizeof(uint32_t) != 0) {
        fprintf(stderr, "Encountered an unaligned memory address 0x%08x. "
                "Halting simulation.\n", addr);
        cpu_state->halted = true;
        return;
    }

    // Try to find the specified address
    mem_segment_t *segment = mem_find_segment(cpu_state, addr);
    if (segment == NULL) {
        fprintf(stderr, "Encountered invalid memory address 0x%08x. Halting "
                "simulation.\n", addr);
        cpu_state->halted = true;
        return;
    }

    // Write the value out in little-endian order
    mem_write_word(segment, addr, value);
    return;
}

/*----------------------------------------------------------------------------
 * Shell Interface Functions
 *----------------------------------------------------------------------------*/

/**
 * Allocates the memory for the given segment, which will have segment->size
 * bytes in it. Exits on error
 **/
static void malloc_mem_segment(mem_segment_t *segment)
{
    segment->mem = malloc(segment->size * sizeof(segment->mem[0]));
    if (segment->mem == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for processor memory "
                "segment.\n");
        exit(ENOMEM);
    }
    return;
}

/**
 * Loads the memory segment from its corresponding data (binary) file. The size
 * of this file cannot exceed the max_size for the memory segment.
 **/
static int load_mem_segment(mem_segment_t *segment, char *data_path)
{
    // Try to open the data file
    FILE *data_file = fopen(data_path, "r");
    if (data_file == NULL) {
        int rc = -errno;
        fprintf(stderr, "Error: %s: Unable to open file: %s.\n", data_path,
                strerror(errno));
        return rc;
    }

    // Determine the size of the memory segment in the file.
    fseek(data_file, 0, SEEK_END);
    segment->size = ftell(data_file);
    rewind(data_file);
    if (segment->size == 0) {
        return 0;
    }

    // Allocate memory for the segment only if the size does not exceed the max
    if (segment->size > segment->max_size) {
        fprintf(stderr, "Error: %s: File is too large for memory segment.\n",
                data_path);
        fclose(data_file);
        return -EFBIG;
    } else if (segment->size % sizeof(uint32_t) != 0) {
        fprintf(stderr, "Error: %s: File size is not aligned to 4 bytes.\n.",
                data_path);
        fclose(data_file);
        return -EINVAL;
    }
    malloc_mem_segment(segment);

    // Since the data file is binary, load it directly into memory
    int rc = 0;
    size_t elems_read = fread(segment->mem, segment->size, 1, data_file);
    if (elems_read != 1) {
        assert(ferror(data_file) || !feof(data_file));
        rc = -errno;
        fprintf(stderr, "Error: %s: Unable to read memory section file: %s.\n",
                data_path, strerror(errno));

        free(segment->mem);
        segment->mem = NULL;
        segment->size = 0;
    }

    // Close the data file
    fclose(data_file);
    return rc;
}

/**
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
 * Initializes the memory subsystem part of the CPU state.
 *
 * This loads the memory segments from the specified program into the CPU
 * memory, and initializes them to the values specified in their respective data
 * files. Program name should be the path to the executable file (it has no
 * extension).
 **/
int mem_load_program(cpu_state_t *cpu_state, const char *program_path)
{
    // Initialize each memory segment, loading data from the associated file
    int rc = 0;
    for (int i = 0; i < cpu_state->memory.num_segments; i++)
    {
        /* If the memory segment doesn't have a data file, we only allocate it.
         * In this case, the size of the memory segment is max_size. */
        mem_segment_t *segment = &cpu_state->memory.segments[i];
        if (segment->extension == NULL) {
            segment->size = segment->max_size;
            malloc_mem_segment(segment);
            continue;
        }

        /* Otherwise, combine the program path and extension to get the path
         * to the data file, load it, then free the buffer. */
        char *data_path = join_strings(program_path, segment->extension);
        rc = load_mem_segment(segment, data_path);
        free(data_path);

        // Free the other memory segments if we failed to load this one
        if (rc < 0) {
            mem_unload_program(cpu_state);
            break;
        }
    }

    /* Point the PC to the user text segment, the stack pointer (x2) to the
     * stack segment, and the global pointer (x3) to the user data segment. */
    cpu_state->pc = USER_TEXT_START;
    register_write(cpu_state, REG_SP, STACK_END);
    register_write(cpu_state, REG_GP, USER_DATA_START);

    return rc;
}

/**
 * Unloads a program previously loaded by mem_load_program.
 *
 * This cleans up and frees the allocated memory for the processor's memory
 * segments.
 **/
void mem_unload_program(struct cpu_state *cpu_state)
{
    // Free each of the memory segments, if it has an allocated memory segment
    memory_t *memory = &cpu_state->memory;
    for (int i = 0; i < memory->num_segments; i++)
    {
        mem_segment_t *segment = &memory->segments[i];
        if (segment->mem != NULL) {
            free(segment->mem);
            segment->mem = NULL;
            segment->size = 0;
        }
    }

    return;
}

/**
 * Checks if the given memory range [start, end) is valid.
 *
 * Namely, this means that all addresses between start and end are valid.
 **/
bool mem_range_valid(const cpu_state_t *cpu_state, uint32_t start_addr,
        uint32_t end_addr)
{
    assert(start_addr < end_addr);

    /* Iterate over the memory segments, checking if the range is completely
     * contained in any segment. */
    for (int i = 0; i < cpu_state->memory.num_segments; i++)
    {
        const mem_segment_t *segment = &cpu_state->memory.segments[i];
        uint32_t segment_end_addr = segment->base_addr + segment->size;

        if (segment->base_addr <= start_addr && end_addr <= segment_end_addr) {
            return true;
        }
    }

    return false;
}

/**
 * Finds the segment in memory that contains the given address.
 *
 * If the address is not contained within any segment (and is therefore
 * invalid), then NULL is returned.
 **/
mem_segment_t *mem_find_segment(const cpu_state_t *cpu_state, uint32_t addr)
{
    // Iterate over the memory segments, checking if the address lies in them
    for (int i = 0; i < cpu_state->memory.num_segments; i++)
    {
        mem_segment_t *segment = &cpu_state->memory.segments[i];
        uint32_t segment_end_addr = segment->base_addr + segment->size;

        if (segment->base_addr <= addr && addr < segment_end_addr) {
            return segment;
        }
    }

    return NULL;
}

/**
 * Writes the specified value out to the given address in the segment in
 * little-endian order.
 *
 * The address must lie inside the specified segment. Any parts of the word
 * which lie outside of the segment are not written.
 **/
void mem_write_word(mem_segment_t *segment, uint32_t addr, uint32_t value)
{
    assert(segment->base_addr <= addr &&
            addr < segment->base_addr + segment->size);

    // Determine the number of bytes that can be written into the segment
    uint32_t end_addr = segment->base_addr + segment->size;
    int bytes_write = min(sizeof(uint32_t), end_addr - addr);
    uint8_t *mem_addr = &segment->mem[addr - segment->base_addr];

    for (int i = 0; i < bytes_write; i++)
    {
        mem_addr[i] = get_byte(value, i);
    }

    return;
}
