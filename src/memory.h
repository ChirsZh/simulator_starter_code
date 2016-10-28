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

/**
 * mem_write32
 *
 * Reads the value at the specified address in the processor's memory. The
 * function ensures that the value is written in little-endian order. If the
 * address is unaligned or invalid, this function will stop the simulator on an
 * exception.
 **/
uint32_t mem_read32(uint32_t addr);

/**
 * mem_write32
 *
 * Writes the specified value to the given address in the processor's memory.
 * The function ensures that the value is written in little-endian order. If the
 * address is unaligned or invalid, this function will stop the simulator on an
 * exception.
 **/
void mem_write32(uint32_t addr, uint32_t value);

/**
 * mem_init
 *
 * Initializes the memory subsystem part of the CPU state. This only needs to be
 * called by the shell at startup time. The core simulator does not need this
 * function.
 **/
void mem_init();

#endif /* MEMORY_H_ */
