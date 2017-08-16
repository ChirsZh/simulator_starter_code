/**
 * memory_segments.h
 *
 * RISC-V 32-bit Instruction Level Simulator
 *
 * ECE 18-447
 * Carnegie Mellon University
 *
 * This file contains the definitions for the segments in the processor memory.
 *
 * This defines the metadata about each segment in memory, such as its starting
 * address and maximum size. Also, this defines an array that represents all of
 * the available memory segments.
 *
 * Authors:
 *  - 2017: Brandon Perez
 **/

#ifndef MEMORY_SEGMENTS_H_
#define MEMORY_SEGMENTS_H_

// 18-447 Simulator Includes
#include <memory.h>         // Memory segment type

/*----------------------------------------------------------------------------
 * Memory Segment Addresses
 *----------------------------------------------------------------------------*/

// The number of memory segments in the processor
#define NUM_MEM_SEGMENTS    5

// The starting addresses of the user's data and text segments
#define USER_TEXT_START     0x00400000
#define USER_DATA_START     0x10000000

// The starting and ending addresses of the stack segment, and its size
#define STACK_END           0x7ff00000
#define STACK_SIZE          (1 * 1024 * 1024)
#define STACK_START         (STACK_END - STACK_SIZE)

// The starting addresses and sizes of the kernel's data, and text segments
#define KERNEL_TEXT_START   0x80000000
#define KERNEL_DATA_START   0x90000000

/*----------------------------------------------------------------------------
 * Memory Segments
 *----------------------------------------------------------------------------*/

// An array containing metadata about the segments in the processor's memory.
__attribute__((unused))
static mem_segment_t MEMORY_SEGMENTS[NUM_MEM_SEGMENTS] = {
    // The user text memory segment, containing user code
    {
        .base_addr          = USER_TEXT_START,
        .max_size           = USER_DATA_START - USER_TEXT_START,
        .extension          = ".text.bin",
        .name               = "User Text",
    },

    // The user data memory segment, containing user global variables
    {
        .base_addr          = USER_DATA_START,
        .max_size           = STACK_START - USER_DATA_START,
        .extension          = ".data.bin",
        .name               = "User Data",
    },

    /* The stack memory segment, containing local values in the program. This is
     * shared by kernel and user code. */
    {
        .base_addr          = STACK_END - STACK_SIZE,
        .max_size           = STACK_SIZE,
        .extension          = NULL,
        .name               = "Stack"
    },

    // The kernel text segment, containing kernel code
    {
        .base_addr          = KERNEL_TEXT_START,
        .max_size           = KERNEL_DATA_START - KERNEL_TEXT_START,
        .extension          = ".ktext.bin",
        .name               = "Kernel Text",
    },

    // The kernel data segment, containing kernel global variables
    {
        .base_addr          = KERNEL_DATA_START,
        .max_size           = UINT32_MAX - KERNEL_DATA_START,
        .extension          = ".kdata.bin",
        .name               = "Kernel Data",
    },
};

#endif /* MEMORY_SEGMENTS_H_ */
