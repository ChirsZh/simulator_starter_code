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
 * Note that the names of the enumerations are based on the names given in
 * chapter 2 of the RISC-V 2.2 ISA manual.
 *
 * Authors:
 *  - 2016 - 2017: Brandon Perez
 **/

/*----------------------------------------------------------------------------*
 *                          DO NOT MODIFY THIS FILE!                          *
 *          You should only add or change files in the src directory!         *
 *----------------------------------------------------------------------------*/

#ifndef RISCV_ISA_H_
#define RISCV_ISA_H_

/*----------------------------------------------------------------------------
 * Definitions
 *----------------------------------------------------------------------------*/

// The number of registers in the register file
#define RISCV_NUM_REGS      32

/*----------------------------------------------------------------------------
 * Opcodes (All Instruction Types)
 *----------------------------------------------------------------------------*/

// Opcodes for the RISC-V ISA, in the lowest 7 bits of the instruction
typedef enum opcode {
    // Opcode that indicates an integer R-type instruction (register)
    OP_OP                   = 0x33,

    // Opcode that indicates an integer I-type instruction (immediate)
    OP_IMM                  = 0x13,

    // Opcodes for load and store instructions (I-type and S-type)
    OP_LOAD                 = 0x03,
    OP_STORE                = 0x23,

    // Opcodes for U-type instructions (unsigned immediate)
    OP_LUI                  = 0x37,
    OP_AUIPC                = 0x17,

    // Opcodes for jump instructions (UJ-type and I-type)
    OP_JAL                  = 0x6F,
    OP_JALR                 = 0x67,

    // Opcode that indicates a general SB-type instruction (branch)
    OP_BRANCH               = 0x63,

    // Opcode that indicates a special system instruction (I-type)
    OP_SYSTEM               = 0x73,
} opcode_t;

/*----------------------------------------------------------------------------
 * 7-bit Function Codes (R-type and I-type Instructions)
 *----------------------------------------------------------------------------*/

// 7-bit function codes, the highest 7-bits of the instruction
typedef enum riscv_funct7 {
    FUNCT7_INT              = 0x00,     // Typical integer instruction
    FUNCT7_ALT_INT          = 0x20,     // Alternate instruction (sub/sra/srai)
} funct7_t;

/*----------------------------------------------------------------------------
 * R-type Function Codes
 *----------------------------------------------------------------------------*/

// 3-bit function codes for integer R-type instructions
typedef enum riscv_rtype_funct3 {
    FUNCT3_ADD_SUB          = 0x0,      // Add/subtract
    FUNCT3_SLL              = 0x1,      // Shift left logical
    FUNCT3_SLT              = 0x2,      // Set on less than signed
    FUNCT3_SLTU             = 0x3,      // Set on less than unsigned
    FUNCT3_XOR              = 0x4,      // Bit-wise xor
    FUNCT3_SRL_SRA          = 0x5,      // Shift right logical/arithmetic
    FUNCT3_OR               = 0x6,      // Bit-wise or
    FUNCT3_AND              = 0x7,      // Bit-wise and
} rtype_funct3_t;

/*----------------------------------------------------------------------------
 * I-type Function Codes
 *----------------------------------------------------------------------------*/

// 3-bit function codes for integer I-type instructions
typedef enum riscv_itype_int_funct3 {
    FUNCT3_ADDI             = 0x0,      // Add immediate
    FUNCT3_SLTI             = 0x2,      // Set on less than signed
    FUNCT3_SLTIU            = 0x3,      // Set on less than unsigned
    FUNCT3_XORI             = 0x4,      // Bit-wise xor immediate
    FUNCT3_ORI              = 0x6,      // Bit-wise or immediate
    FUNCT3_ANDI             = 0x7,      // Bit-wise and immediate
    FUNCT3_SLLI             = 0x1,      // Shift left logical immediate
    FUNCT3_SRLI_SRAI        = 0x5,      // Shift right logical/arithmetic
} itype_int_funct3_t;

// 3-bit function codes for load instructions (I-type)
typedef enum riscv_itype_load_funct3 {
    FUNCT3_LB               = 0x0,      // Load byte (1 byte) signed
    FUNCT3_LH               = 0x1,      // Load halfword (2 bytes) signed
    FUNCT3_LW               = 0x2,      // Load word (4 bytes)
    FUNCT3_LBU              = 0x4,      // Load byte (1 byte) unsigned
    FUNCT3_LHU              = 0x5,      // Load halfword (2 bytes) unsigned
} itype_load_funct3_t;

// 12-bit function codes for special system instructions (I-type)
typedef enum riscv_itype_funct12 {
    FUNCT12_ECALL           = 0x000,    // Environment call
} itype_funct12_t;

/*----------------------------------------------------------------------------
 * S-type Function Codes
 *----------------------------------------------------------------------------*/

// 3-bit function codes for S-type instructions (store)
typedef enum riscv_stype_funct3 {
    FUNCT3_SB               = 0x0,      // Store byte (1 byte)
    FUNCT3_SH               = 0x1,      // Store halfword (2 bytes)
    FUNCT3_SW               = 0x2,      // Store word (4 bytes)
} stype_funct3_t;

/*----------------------------------------------------------------------------
 * SB-type Function Codes
 *----------------------------------------------------------------------------*/

// 3-bit function codes for SB-type instructions (branch)
typedef enum riscv_sbtype_funct3 {
    FUNCT3_BEQ              = 0x0,      // Branch if equal
    FUNCT3_BNE              = 0x1,      // Branch if not equal
    FUNCT3_BLT              = 0x4,      // Branch if less than (signed)
    FUNCT3_BGE              = 0x5,      // Branch if greater than or equal
    FUNCT3_BLTU             = 0x6,      // Branch if less than (unsigned)
    FUNCT3_BGEU             = 0x7,      // Branch if greater than or equal
} sbtype_funct3_t;

/*----------------------------------------------------------------------------
 * ISA Register Names
 *----------------------------------------------------------------------------*/

// Enumeration of the registers in the ISA
typedef enum logic {
    REG_X0              = 0,            // ISA Register 0, hardwired to 0
    REG_X1              = 1,            // ISA Register 1
    REG_X2              = 2,            // ISA Register 2
    REG_X3              = 3,            // ISA Register 3
    REG_X4              = 4,            // ISA Register 4
    REG_X5              = 5,            // ISA Register 5
    REG_X6              = 6,            // ISA Register 6
    REG_X7              = 7,            // ISA Register 7
    REG_X8              = 8,            // ISA Register 8
    REG_X9              = 9,            // ISA Register 9
    REG_X10             = 10,           // ISA Register 10
    REG_X11             = 11,           // ISA Register 11
    REG_X12             = 12,           // ISA Register 12
    REG_X13             = 13,           // ISA Register 13
    REG_X14             = 14,           // ISA Register 14
    REG_X15             = 15,           // ISA Register 15
    REG_X16             = 16,           // ISA Register 16
    REG_X17             = 17,           // ISA Register 17
    REG_X18             = 18,           // ISA Register 18
    REG_X19             = 19,           // ISA Register 19
    REG_X20             = 20,           // ISA Register 20
    REG_X21             = 21,           // ISA Register 21
    REG_X22             = 22,           // ISA Register 22
    REG_X23             = 23,           // ISA Register 23
    REG_X24             = 24,           // ISA Register 24
    REG_X25             = 25,           // ISA Register 25
    REG_X26             = 26,           // ISA Register 26
    REG_X27             = 27,           // ISA Register 27
    REG_X28             = 28,           // ISA Register 28
    REG_X29             = 29,           // ISA Register 29
    REG_X30             = 30,           // ISA Register 30
    REG_X31             = 31,           // ISA Register 31
} riscv_isa_reg_t;

#endif /* RISCV_ISA_H_ */
