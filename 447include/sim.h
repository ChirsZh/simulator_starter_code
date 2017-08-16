/**
 * sim.h
 *
 * RISC-V 32-bit Instruction Level Simulator
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

// Standard Includes
#include <stdbool.h>                    // Boolean type and definitions
#include <stdint.h>                     // Fixed-size integral types

// Local Includes
#include "riscv_isa.h"                  // RISC-V ISA, the number of registers
#include "memory.h"                     // Interface to the processor memory

/*----------------------------------------------------------------------------
 * Definitions
 *----------------------------------------------------------------------------*/

// A structure representing all of the state in a processor.
typedef struct cpu_state {
    bool verbose_mode;                  // Indicates if verbose mode is active
    bool halted;                        // Indicates if the CPU is halted
    int cycle;                          // Number of processor cycles
    uint32_t pc;                        // Current program counter
    char *program;                      // Name of the currently loaded program
    memory_t memory;                    // Processor memory segments
    uint32_t registers[RISCV_NUM_REGS]; // CPU register file
} cpu_state_t;

/*----------------------------------------------------------------------------
 * Interface
 *----------------------------------------------------------------------------*/

/**
 * Simulates a single cycle on the CPU, updating the CPU's state as needed.
 *
 * This is the core part of the simulator. This simulates the current
 * instruction pointed to by the PC. This performs the necessary actions for the
 * instruction, and updates the CPU state appropriately.
 *
 * You implement this function.
 *
 * Inputs:
 *  - cpu_state     The current state of the CPU being simulated.
 *
 * Outputs:
 *  - cpu_state     The next state of the CPU being simulated. This function
 *                  updates the fields of the state as needed by the current
 *                  instruction to simulate it.
 **/
void process_instruction(cpu_state_t *cpu_state);

#endif /* SIM_H_ */
