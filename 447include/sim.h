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
 * Authors:
 *  - 2016 - 2017: Brandon Perez
 **/

/*----------------------------------------------------------------------------*
 *                          DO NOT MODIFY THIS FILE!                          *
 *          You should only add or change files in the src directory!         *
 *----------------------------------------------------------------------------*/

#ifndef SIM_H_
#define SIM_H_

#include <stdbool.h>                    // Boolean type and definitions
#include <stdint.h>                     // Fixed-size integral types

#include "riscv_abi.h"                  // RISC-V ABI, the number of registers
#include "memory.h"                     // Interface to the processor memory

/*----------------------------------------------------------------------------
 * Definitions
 *----------------------------------------------------------------------------*/

// A structure representing all of the state in a processor.
typedef struct cpu_state {
    bool halted;                        // Indicates if the CPU is halted
    int cycle;                          // Number of processor cycles
    uint32_t pc;                        // Current program counter
    char *program;                      // Name of the currently loaded program
    memory_t memory;                    // Processor memory segments
    uint32_t regs[RISCV_NUM_REGS];      // CPU register file
} cpu_state_t;

/*----------------------------------------------------------------------------
 * Interface
 *----------------------------------------------------------------------------*/

/**
 * process_instruction
 *
 * The core part of the simulator. Simulates a single cycle on the CPU,
 * simulating the current instruction pointed to by the PC. Updates the CPU
 * state appropriately.
 *
 * You implement this function.
 **/
void process_instruction(cpu_state_t *cpu_state);

#endif /* SIM_H_ */
