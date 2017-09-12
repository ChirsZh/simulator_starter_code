/**
 * riscv_abi.h
 *
 * RISC-V 32-bit Instruction Level Simulator
 *
 * ECE 18-447
 * Carnegie Mellon University
 *
 * This file contains the definitions for the RISC-V application binary
 * interface (ABI), which are namely the register aliases for application
 * registers, such as temporaries, the stack pointer, etc.
 *
 * Note that the names of the enumerations are based on the names and
 * assignments in chapter 20 of the RISC-V 2.2 ISA manual.
 *
 * Authors:
 *  - 2016 - 2017: Brandon Perez
 **/

/*----------------------------------------------------------------------------*
 *                          DO NOT MODIFY THIS FILE!                          *
 *          You should only add or change files in the src directory!         *
 *----------------------------------------------------------------------------*/

#ifndef RISCV_ABI_H_
#define RISCV_ABI_H_

// Standard Includes
#include <stdint.h>         // Fixed-size integral types

// Local Includes
#include "riscv_isa.h"      // RISC-V ISA registers

/*----------------------------------------------------------------------------
 * Definitions
 *----------------------------------------------------------------------------*/

/* The value that must be passed in register a0 (x10) to the ECALL instruction
 * to halt the processor and stop the simulator. */
static const uint32_t ECALL_ARG_HALT    = 0xa;

// Aliases for the register in the application binary interface (ABI)
typedef enum {
    REG_ZERO    = REG_X0,   // Zero register, hardwired to 0
    REG_RA      = REG_X1,   // Return address register (caller-saved)
    REG_SP      = REG_X2,   // Stack pointer register (callee-saved)
    REG_GP      = REG_X3,   // Global pointer register (points to data section)
    REG_TP      = REG_X4,   // Thread pointer (points to thread-local data)
    REG_T0      = REG_X5,   // Temporary register 0 (caller-saved)
    REG_T1      = REG_X6,   // Temporary register 0 (caller-saved)
    REG_T2      = REG_X7,   // Temporary register 0 (caller-saved)
    REG_S0_FP   = REG_X8,   // Saved 0/stack frame pointer (callee-saved)
    REG_S1      = REG_X9,   // Saved register 1 (callee-saved)
    REG_A0      = REG_X10,  // Function argument/return value 0 (caller-saved)
    REG_A1      = REG_X11,  // Function argument/return value 1 (caller-saved)
    REG_A2      = REG_X12,  // Function argument 2 (caller-saved)
    REG_A3      = REG_X13,  // Function argument 3 (caller-saved)
    REG_A4      = REG_X14,  // Function argument 4 (caller-saved)
    REG_A5      = REG_X15,  // Function argument 5 (caller-saved)
    REG_A6      = REG_X16,  // Function argument 6 (caller-saved)
    REG_A7      = REG_X17,  // Function argument 7 (caller-saved)
    REG_S2      = REG_X18,  // Saved register 2 (callee-saved)
    REG_S3      = REG_X19,  // Saved register 3 (callee-saved)
    REG_S4      = REG_X20,  // Saved register 4 (callee-saved)
    REG_S5      = REG_X21,  // Saved register 5 (callee-saved)
    REG_S6      = REG_X22,  // Saved register 6 (callee-saved)
    REG_S7      = REG_X23,  // Saved register 7 (callee-saved)
    REG_S8      = REG_X24,  // Saved register 8 (callee-saved)
    REG_S9      = REG_X25,  // Saved register 9 (callee-saved)
    REG_S10     = REG_X26,  // Saved register 10 (callee-saved)
    REG_S11     = REG_X27,  // Saved register 11 (callee-saved)
    REG_T3      = REG_X28,  // Temporary register 3 (caller-saved)
    REG_T4      = REG_X29,  // Temporary register 4 (caller-saved)
    REG_T5      = REG_X30,  // Temporary register 5 (caller-saved)
    REG_T6      = REG_X31,  // Temporary register 6 (caller-saved)
} riscv_abi_reg_t;

#endif /* RISCV_ABI_H_ */
