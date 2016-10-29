/**
 * sim.h
 *
 * RISC-V 32-bit Instruction Level Simulator.
 *
 * ECE 18-447
 * Carnegie Mellon University
 *
 * This file contains the interface to the core part of the simulator. The core
 * simulator handles carrying out the actions required by instructions.
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

#ifndef SIM_H_
#define SIM_H_

#include <stdbool.h>                // Boolean type and definitions
#include <stdint.h>                 // Fixed-size integral types

#include "riscv_isa.h"              // RISC-V ISA, the number of registers
#include "memory.h"                 // Interface to the processor memory

typedef struct cpu_state {
    bool running;                   // Indicates if the CPU is running
    int instr_count;                // Number of simulated instructions
    uint32_t pc;                    // Current program counter
    int num_mem_regions;            // Number of memory regions
    mem_region_t **mem_regions;     // Processor memory regions
    uint32_t regs[RISCV_NUM_REGS];  // CPU register file
} cpu_state_t;

/**
 * process_instruction
 *
 * The core part of the simulator. Simulates a single cycle on the CPU,
 * simulating the current instruction pointed to by the PC. Updates the CPU
 * state appropiately.
 *
 * You implement this function.
 **/
void process_instruction(cpu_state_t *cpu_state);

#endif /* SIM_H_ */
