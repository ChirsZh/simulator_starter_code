/**
 * riscv_isa.h
 *
 * RISC-V 32-bit Instruction Level Simulator
 *
 * ECE 18-447
 * Carnegie Mellon University
 *
 * This contains the definitions for the RISC-V opcodes and function codes for
 * the instructions that must be implemented by the simulator.
 *
 * Note that the names of the enumerations are based on the names given in the
 * RISC-V 2.1 ISA manual, sections 2 and 5.
 *
 * Authors:
 *  - 2016: Brandon Perez
 **/

/*----------------------------------------------------------------------------*
 *                          DO NOT MODIFY THIS FILE!                          *
 *                 You should only add files or change sim.c!                 *
 *----------------------------------------------------------------------------*/

#ifndef RISCV_ISA_H_
#define RISCV_ISA_H_

// Opcodes for the RISC-V ISA, in the lowest 7 bits of the instruction
typedef enum riscv_op {
    // Opcodes for U-type instructions (unsigned immediate)
    OP_LUI                  = 0x37,
    OP_AUIPC                = 0x17,

    // Opcodes for jump instructions (UJ-type and I-type)
    OP_JAL                  = 0x6F,
    OP_JALR                 = 0x67,

    // Opcode that indicates a general SB-type instruction (branch)
    OP_BRANCH               = 0x63,

    // Opcodes for load and store instructions (I-type and S-type)
    OP_LOAD                 = 0x03,
    OP_STORE                = 0x23,

    // Opcode that indicates an integer I-type instruction (immediate)
    OP_IMM                  = 0x13,

    // Opcode that indicates an integer R-type instruction (register)
    OP_OP                   = 0x33,

    // Opcode that indicates a special system instruction (R-type)
    OP_SYSTEM               = 0x73,
} riscv_op_t;

// 3-bit function codes for SB-type instructions (branch)
typedef enum riscv_sbtype_funct3 {
    FUNCT3_BEQ              = 0x0,
    FUNCT3_BNE              = 0x1,
    FUNCT3_BLT              = 0x4,
    FUNCT3_BGE              = 0x5,
    FUNCT3_BLTU             = 0x6,
    FUNCT3_BGEU             = 0x7,
} riscv_sbtype_funct3_t;

// 3-bit function codes for load instructions (I-type)
typedef enum riscv_load_funct3 {
    FUNCT3_LB               = 0x0,
    FUNCT3_LH               = 0x1,
    FUNCT3_LW               = 0x2,
    FUNCT3_LBU              = 0x4,
    FUNCT3_LHU              = 0x5,
} riscv_load_funct3_t;

// 3-bit function codes for S-type instructions (store)
typedef enum riscv_store_funct3 {
    FUNCT3_SB               = 0x0,
    FUNCT3_SH               = 0x1,
    FUNCT3_SW               = 0x2,
} riscv_store_funct3_t;

// 3-bit function codes for integer I-type instructions
typedef enum riscv_itype_funct3 {
    FUNCT3_ADDI             = 0x0,
    FUNCT3_SLTI             = 0x2,
    FUNCT3_SLTIU            = 0x3,
    FUNCT3_XORI             = 0x4,
    FUNCT3_ORI              = 0x6,
    FUNCT3_ANDI             = 0x7,
    FUNCT3_SLLI             = 0x1,
    FUNCT3_SRLI_SRAI        = 0x5,
} riscv_itype_funct3_t;

// 3-bit function codes for integer R-type instruction
typedef enum riscv_rtype_funct3 {
    // Basic integer operations
    FUNCT3_ADD_SUB          = 0x0,
    FUNCT3_SLL              = 0x1,
    FUNCT3_SLT              = 0x2,
    FUNCT3_SLTU             = 0x3,
    FUNCT3_XOR              = 0x4,
    FUNCT3_SRL_SRA          = 0x5,
    FUNCT3_OR               = 0x6,
    FUNCT3_AND              = 0x7,

    // Multiply and divide operations
    FUNCT3_MUL              = 0x0,
    FUNCT3_MULH             = 0x1,
    FUNCT3_MULHSU           = 0x2,
    FUNCT3_MULHU            = 0x3,
    FUNCT3_DIV              = 0x4,
    FUNCT3_DIVU             = 0x5,
    FUNCT3_REM              = 0x6,
    FUNCT3_REMU             = 0x7,
} riscv_rtype_funct3_t;

// 7-bit function codes for integer I-type instructions
typedef enum riscv_itype_funct7 {
    FUNCT7_SRLI             = 0x00,
    FUNCT7_SRAI             = 0x20,
} riscv_itype_funct7_t;

// 7-bit function codes for integer R-type instructions
typedef enum riscv_rtype_funct7 {
    // Function codes for basic integer instructions
    FUNCT7_ADD              = 0x00,
    FUNCT7_SUB              = 0x20,
    FUNCT7_SRL              = 0x00,
    FUNCT7_SRA              = 0x20,

    // Function code that indicates this is a multiply or divide instruction
    FUNCT7_MUL_DIV          = 0x01,
} riscv_rtype_funct7_t;

// 12-bit function codes for special system instructions (R-type)
typedef enum rsicv_sys_funct12 {
    FUNCT12_ECALL           = 0x000,
} riscv_sys_funct12_t;

#endif /* RISCV_ISA_H_ */
