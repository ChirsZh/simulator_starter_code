/**
 * riscv_register_names.h
 *
 * RISC-V 32-bit Instruction Level Simulator
 *
 * ECE 18-447
 * Carnegie Mellon University
 *
 * This file contains the definitions for the names of the registers in the
 * RISC-V ISA.
 *
 * This is the definition of a structure that has both the ISA and ABI variants
 * of the register name, along with an array that contains the names for all
 * the registers. These names are used to generate the register dumps.
 *
 * Authors:
 *  - 2016 - 2017: Brandon Perez
 **/

#ifndef RISCV_REGISTER_NAMES_H_
#define RISCV_REGISTER_NAMES_H_

// Structure representing the naming information about a register
typedef struct register_name {
    const char *isa_name;       // The ISA name for a register (x0..x31)
    const char *abi_name;       // The ABI name for a register (sp, t0, etc.)
} register_name_t;

// An array of all the naming information for a register, and its ABI aliases
static const register_name_t RISCV_REGISTER_NAMES[RISCV_NUM_REGS] = {
    { .isa_name = "x0",  .abi_name = "zero", },
    { .isa_name = "x1",  .abi_name = "ra", },
    { .isa_name = "x2",  .abi_name = "sp", },
    { .isa_name = "x3",  .abi_name = "gp", },
    { .isa_name = "x4",  .abi_name = "tp", },
    { .isa_name = "x5",  .abi_name = "t0", },
    { .isa_name = "x6",  .abi_name = "t1", },
    { .isa_name = "x7",  .abi_name = "t2", },
    { .isa_name = "x8",  .abi_name = "s0/fp", },
    { .isa_name = "x9",  .abi_name = "s1", },
    { .isa_name = "x10", .abi_name = "a0", },
    { .isa_name = "x11", .abi_name = "a1", },
    { .isa_name = "x12", .abi_name = "a2", },
    { .isa_name = "x13", .abi_name = "a3", },
    { .isa_name = "x14", .abi_name = "a4", },
    { .isa_name = "x15", .abi_name = "a5", },
    { .isa_name = "x16", .abi_name = "a6", },
    { .isa_name = "x17", .abi_name = "a7", },
    { .isa_name = "x18", .abi_name = "s2", },
    { .isa_name = "x19", .abi_name = "s3", },
    { .isa_name = "x20", .abi_name = "s4", },
    { .isa_name = "x21", .abi_name = "s5", },
    { .isa_name = "x22", .abi_name = "s6", },
    { .isa_name = "x23", .abi_name = "s7", },
    { .isa_name = "x24", .abi_name = "s8", },
    { .isa_name = "x25", .abi_name = "s9", },
    { .isa_name = "x26", .abi_name = "s10", },
    { .isa_name = "x27", .abi_name = "s11", },
    { .isa_name = "x28", .abi_name = "t3", },
    { .isa_name = "x29", .abi_name = "t4", },
    { .isa_name = "x30", .abi_name = "t5", },
    { .isa_name = "x31", .abi_name = "t6", },
};

#endif /* RISCV_REGISTER_NAMES_H_ */
