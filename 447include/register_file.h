/**
 * register_file.h
 *
 * RISC-V 32-bit Instruction Level Simulator.
 *
 * ECE 18-447
 * Carnegie Mellon University
 *
 * This file contains the interface to the processor's register file.
 *
 * This handles abstracting the register file to the core simulator functions.
 * These functions should be used to read and write register values.
 *
 * Authors:
 *  - 2017: Brandon Perez
 **/

#ifndef REGISTER_FILE_H_
#define REGISTER_FILE_H_

// Standard Includes
#include <stdint.h>                 // Fixed-size integral types
#include <assert.h>                 // Assert macro

// 18-447 Simulator Includes
#include <sim.h>                    // Definition of the CPU state
#include <riscv_abi.h>              // Definition of RISC-V register type

/*----------------------------------------------------------------------------
 * Interface
 *----------------------------------------------------------------------------*/

/**
 * Reads the value of source register rs from the processor's register file.
 *
 * The source register must be a valid register number, so it must fall between
 * 0 and RISCV_NUM_REGS.
 *
 * Inputs:
 *  - cpu_state     The CPU state structure for the processor.
 *  - rs            The source register from which to read the value.
 *
 * Outputs:
 *  - return        The value of register rs in the register file.
 **/
uint32_t register_read(const cpu_state_t *cpu_state, riscv_reg_t rs);

/**
 * Updates the destination register rd with the given value.
 *
 * The destination register must be a valid register number, so it must fall
 * between 0 and RISCV_NUM_REGS. Writes to register 0 are ignored, as per the
 * RISC-V ISA.
 *
 * Inputs:
 *  - cpu_state     The CPU state structure for the processor.
 *  - rd            The destination register to which to write the value.
 *  - value         The value with which to update the destination register.
 *
 * Outputs:
 *  - cpu_state     If rd is not 0, then the appropriate entry in the registers
 *                  array is updated.
 **/
void register_write(cpu_state_t *cpu_state, riscv_reg_t rd, uint32_t value);

#endif /* REGISTER_FILE_H_ */
