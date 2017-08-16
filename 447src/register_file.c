/**
 * register_file.c
 *
 * RISC-V 32-bit Instruction Level Simulator
 *
 * ECE 18-447
 * Carnegie Mellon University
 *
 * This file contains the backend for the processor's register file, which
 * handles abstracting the register file to the core simulator functions.
 *
 * Authors:
 *  - 2017: Brandon Perez
 **/

// Standard Includes
#include <stdint.h>                 // Fixed-size integral types
#include <assert.h>                 // Assert macro

// 18-447 Simulator Includes
#include <sim.h>                    // Definition of the CPU state
#include <register_file.h>          // This file's interface
#include <riscv_abi.h>              // Definition of RISC-V register type

// Local Includes
#include "libc_extensions.h"        // Array length function

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
uint32_t register_read(const cpu_state_t *cpu_state, riscv_reg_t rs)
{
    assert((riscv_reg_t)0 <= rs);
    assert(rs <= (riscv_reg_t)(array_len(cpu_state->registers) - 1));

    return cpu_state->registers[rs];
}

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
void register_write(cpu_state_t *cpu_state, riscv_reg_t rd, uint32_t value)
{
    assert((riscv_reg_t)0 <= rd);
    assert(rd <= (riscv_reg_t)(array_len(cpu_state->registers) - 1));

    if (rd != 0) {
        cpu_state->registers[rd] = value;
    }
    return;
}
