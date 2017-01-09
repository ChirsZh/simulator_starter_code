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
 * assignments in the RISC-V 2.1 ISA manual, section 20.2.
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

/*----------------------------------------------------------------------------
 * Definitions
 *----------------------------------------------------------------------------*/

// The number of registers in the register file
#define RISCV_NUM_REGS      32

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

/* The value that must be passed in register a0 (x10) to the ECALL instruction
 * to halt the processor and stop the simulator. */
#define ECALL_ARG_HALT      0xa

// Aliases for the register in the application binary interface (ABI)
typedef enum riscv_abi_reg {
    REG_ZERO    = 0,        // Zero register, hardwired to 0
    REG_RA      = 1,        // Return address register (caller-saved)
    REG_SP      = 2,        // Stack pointer register (callee-saved)
    REG_GP      = 3,        // Global pointer register (points to data section)
    REG_TP      = 4,        // Thread pointer (points to thread-local data)
    REG_T0      = 5,        // Temporary register 0 (caller-saved)
    REG_T1      = 6,        // Temporary register 0 (caller-saved)
    REG_T2      = 7,        // Temporary register 0 (caller-saved)
    REG_S0_FP   = 8,        // Saved 0/stack frame pointer (callee-saved)
    REG_S1      = 9,        // Saved register 1 (callee-saved)
    REG_A0      = 10,       // Function argument/return value 0 (caller-saved)
    REG_A1      = 11,       // Function argument/return value 1 (caller-saved)
    REG_A2      = 12,       // Function argument 2 (caller-saved)
    REG_A3      = 13,       // Function argument 3 (caller-saved)
    REG_A4      = 14,       // Function argument 4 (caller-saved)
    REG_A5      = 15,       // Function argument 5 (caller-saved)
    REG_A6      = 16,       // Function argument 6 (caller-saved)
    REG_A7      = 17,       // Function argument 7 (caller-saved)
    REG_S2      = 18,       // Saved register 2 (callee-saved)
    REG_S3      = 19,       // Saved register 3 (callee-saved)
    REG_S4      = 20,       // Saved register 4 (callee-saved)
    REG_S5      = 21,       // Saved register 5 (callee-saved)
    REG_S6      = 22,       // Saved register 6 (callee-saved)
    REG_S7      = 23,       // Saved register 7 (callee-saved)
    REG_S8      = 24,       // Saved register 8 (callee-saved)
    REG_S9      = 25,       // Saved register 9 (callee-saved)
    REG_S10     = 26,       // Saved register 10 (callee-saved)
    REG_S11     = 27,       // Saved register 11 (callee-saved)
    REG_T3      = 28,       // Temporary register 3 (caller-saved)
    REG_T4      = 29,       // Temporary register 4 (caller-saved)
    REG_T5      = 30,       // Temporary register 5 (caller-saved)
    REG_T6      = 31,       // Temporary register 6 (caller-saved)
} abi_reg_t;

#endif /* RISCV_ABI_H_ */
