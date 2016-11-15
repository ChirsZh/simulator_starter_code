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
#include <string.h>         // String manipulation functions and memset

#include "sim.h"            // Interface to the core simulator
#include "memory.h"         // This file's interface

/*----------------------------------------------------------------------------
 * Internal Definitions
 *----------------------------------------------------------------------------*/

// Macro to get the length of a statically allocated array
#define array_len(x) (sizeof(x) / sizeof(x[0]))

// The starting addresses of the user's stack, data, and text segments
#define USER_TEXT_START     0x00400000
#define USER_DATA_START     0x10000000
#define USER_STACK_START    0x7ff00000

// The starting addresses and sizes of the kernel's data, and text segments
#define KERNEL_TEXT_START   0x80000000
#define KERNEL_DATA_START   0x90000000

// The length of a line in a hex file, including the newline character
const size_t MEM_FILE_LINE_LEN = 8 + 1;

// The starting addresses of each memory segment in the processor
static uint32_t MEM_REGION_STARTS[] = {
    USER_TEXT_START,
    USER_DATA_START,
    USER_STACK_START,
    KERNEL_TEXT_START,
    KERNEL_DATA_START,
};

// The sizes of each memory segment in the processor
static const uint32_t MEM_REGION_MAX_SIZES[] = {
    USER_DATA_START - USER_TEXT_START,
    USER_STACK_START - USER_DATA_START,
    KERNEL_TEXT_START - USER_STACK_START,
    KERNEL_DATA_START - KERNEL_TEXT_START,
    UINT32_MAX - KERNEL_DATA_START,
};

// The file extensions for each memory region
static const char *const MEM_FILE_EXTENSIONS[] = {
    ".text.hex",
    ".data.hex",
    ".stack.hex",
    ".ktext.hex",
    ".kdata.hex",
};

// The number of memory segments in the processor
static const int NUM_MEM_REGIONS = array_len(MEM_REGION_STARTS);

/*----------------------------------------------------------------------------
 * Shared Helper Functions
 *----------------------------------------------------------------------------*/

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

/**
 * mem_write_word
 *
 * Writes the spcified value out to the given address in little endian order.
 * The address must be aligned on a 4-byte boundary.
 **/
static void write_little_endian(uint8_t *mem_addr, uint32_t value)
{
    assert((uintptr_t)mem_addr % sizeof(value) == 0);

    mem_addr[0] = get_byte(value, 0);
    mem_addr[1] = get_byte(value, 1);
    mem_addr[2] = get_byte(value, 2);
    mem_addr[3] = get_byte(value, 3);

    return;
}

/**
 * mem_read_word
 *
 * Reads the specified value out from the given address in little endian order.
 * The address must be aligned on a 4-byte boundary
 **/
static uint32_t read_little_endian(const uint8_t *mem_addr)
{
    assert((uintptr_t)mem_addr % sizeof(uint32_t) == 0);

    uint32_t value = 0;
    value |= set_byte(mem_addr[0], 0);
    value |= set_byte(mem_addr[1], 1);
    value |= set_byte(mem_addr[2], 2);
    value |= set_byte(mem_addr[3], 3);

    return value;
}

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
        cpu_state->halted = true;
        return 0;
    }

    return read_little_endian(mem_addr);
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
    uint8_t *mem_addr = find_address(cpu_state, addr);
    if (mem_addr == NULL) {
        fprintf(stderr, "Encountered invalid memory address 0x%08x. Ending "
                "simulation.\n", addr);
        cpu_state->halted = true;
        return;
    }

    // Write the value out in little-endian order
    write_little_endian(mem_addr, value);
    return;
}

/*----------------------------------------------------------------------------
 * Shell Interface Functions
 *----------------------------------------------------------------------------*/

/**
 * load_file
 *
 * Loads the given hex file into the specified memory region, allocating the
 * amount of memory needed. The hex file is parsed as an ASCII text file, with
 * one 32-bit hexadecimal value per line.
 **/
static int load_hex_file(mem_region_t *mem_region, FILE *hex_file,
        const char *hex_path)
{
    // Check that the size does not exceed the max, and allocate memory for it
    mem_region->mem = malloc(mem_region->size * sizeof(mem_region->mem[0]));
    if (mem_region->mem == NULL)  {
        errno = ENOMEM;
        perror("Error: Unable to allocate the processor memory regions");
        return -errno;
    }

    // Read the first line of the file, allowing it to allocate a line buffer
    char *line = NULL;
    size_t buf_size = 0;
    ssize_t len = getline(&line, &buf_size, hex_file);

    // Read each line of the file, parse it as hex, and load it into memory
    int line_num = 0;
    size_t offset = 0;
    while (len >= 0)
    {
        // Strip the newline and parse the line as a 32-bit hexadecimal integer
        errno = 0;
        line[len] = '\0';
        unsigned long long value = strtoull(line, NULL, 16);
        if (errno != 0) {
            len = -errno;
            fprintf(stderr, "Error: %s: Line %d: Unable to parse '%s' as an "
                    "32-bit hexadecimal integer.\n", hex_path, line_num, line);
            break;
        } else if (value > (unsigned long long)UINT32_MAX) {
            len = -ERANGE;
            fprintf(stderr, "Error: %s: Line %d: Integer '%s' is too large to "
                    "fit in a 32-bit value.\n", hex_path, line_num, line);
            break;
        }

        // Write the value to memory, and increment the offset into memory
        write_little_endian(&mem_region->mem[offset], (uint32_t)value);
        offset += sizeof(uint32_t);

        // Get the next line of the file
        len = getline(&line, &buf_size, hex_file);
        line_num += 1;
    }

    // If we failed while parsing, then free the memory buffer
    len = (feof(hex_file)) ? 0 : len;
    if (len < 0) {
        free(mem_region->mem);
        mem_region->mem = NULL;
    }

    // Free the line buffer allocated by getline
    free(line);
    return len;
}

/**
 * load_program
 *
 * Loads the given memory region from its corresponding hex file. The hex file
 * is the concatenation of the specified program path and the extension. The
 * file cannot exceed the given maximum size for this memory region.
 **/
static int load_mem_region(mem_region_t *mem_region, const char *program_path,
        const char *extension, uint32_t max_size)
{
    // Allocate a new string to hold the program path and extension
    size_t len = strlen(program_path) + strlen(extension);
    char *hex_path = malloc(len * sizeof(*hex_path));
    if (hex_path == NULL) {
        errno = ENOMEM;
        perror("Error: Unable to allocate the hex file path string");
        exit(errno);
    }

    // Combine the program path and extension to get the hex file's path
    strcpy(hex_path, program_path);
    strcat(hex_path, extension);

    // Try to open the file
    FILE *hex_file = fopen(hex_path, "r");
    if (hex_file == NULL) {
        int rc = -errno;
        fprintf(stderr, "Error: %s: Unable to open file", hex_path);
        errno = rc;
        perror("");
        free(hex_path);
        return rc;
    }

    /* Determine the size of the memory region in the file in bytes, accounting
     * for the fact that hex file is an ASCII text file. */
    fseek(hex_file, 0, SEEK_END);
    size_t file_size = ftell(hex_file);
    rewind(hex_file);
    size_t num_lines = file_size / MEM_FILE_LINE_LEN;
    mem_region->size = num_lines * sizeof(uint32_t);

    // If the memory region size does not exceed the max, load the file
    int rc;
    if (mem_region->size <= max_size) {
        rc = load_hex_file(mem_region, hex_file, hex_path);
    } else {
        fprintf(stderr, "Error: %s: File is too large for memory region.\n",
                hex_path);
        rc = -EFBIG;
    }

    // Free the buffer for the hex path
    free(hex_path);
    return rc;
}


/**
 * mem_load_program
 *
 * Initializes the memory subsystem part of the CPU state. This loads the memory
 * regions from the specified program into the CPU memory, and initializes them
 * to the values specified in the respective hex files. Program name should be
 * the path to the assembly file without the extension.
 **/
int mem_load_program(cpu_state_t *cpu_state, const char *program_path)
{
    assert(program_path != NULL);

    // Allocate the memory region metadata for the processor
    cpu_state->num_mem_regions = NUM_MEM_REGIONS;
    size_t region_size = sizeof(cpu_state->mem_regions[0]);
    cpu_state->mem_regions = calloc(cpu_state->num_mem_regions, region_size);
    if (cpu_state->mem_regions == NULL) {
        errno = ENOMEM;
        perror("Error: Unable to allocate memory regions structure");
        exit(errno);
    }

    // Initialize each memory region, loading data from the associated hex file
    for (int i = 0; i < cpu_state->num_mem_regions; i++)
    {
        // Setup the metadata for the memory region
        mem_region_t *mem_region = &cpu_state->mem_regions[i];
        mem_region->base_addr = MEM_REGION_STARTS[i];

        // Try to load the memory section from the associated hex file
        const char *extension = MEM_FILE_EXTENSIONS[i];
        size_t max_size = MEM_REGION_MAX_SIZES[i];
        int rc = load_mem_region(mem_region, program_path, extension, max_size);
        if (rc < 0) {
            mem_unload_program(cpu_state);
            return rc;
        }
    }

    // Set the PC to the start of the text section
    cpu_state->pc = USER_TEXT_START;
    return 0;
}

/**
 * mem_unload_program
 *
 * Unloads a program previously loaded by mem_load_program. This cleans up and
 * frees the allocated memory for the processor's memory region.
 **/
void mem_unload_program(cpu_state_t *cpu_state)
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
