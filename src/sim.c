/**
 * sim.c
 *
 * RISC-V 32-bit Instruction Level Simulator
 *
 * ECE 18-447
 * Carnegie Mellon University
 *
 * This is core part of the simulator, and is responsible for simulating the
 * current instruction, and updating the CPU state appropriately.
 *
 * This is where you can start add code and make modifications to implement the
 * rest of the instructions. You can add any additional files or change and
 * delete files as you need to implement the simulator, under the src directory.
 *
 * The Makefile will automatically find any files you add, provided they are
 * under the src directory and have either a *.c or *.h extension.
 **/

/*----------------------------------------------------------------------------*
 *  You may edit this file and add or change any files in the src directory.  *
 *----------------------------------------------------------------------------*/

// Standard Includes
#include <stdio.h>              // Printf and related functions
#include <stdbool.h>            // Boolean type and definitions

// 18-447 Simulator Includes
#include <riscv_isa.h>          // Definition of RISC-V opcodes, ISA registers
#include <riscv_abi.h>          // ABI registers and definitions
#include <sim.h>                // Definitions for the simulator
#include <memory.h>             // Interface to the processor memory
#include <register_file.h>      // Interface to the register file

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
void process_instruction(cpu_state_t *cpu_state)
{
    // Fetch the 4-bytes for the current instruction
    uint32_t instr = mem_read32(cpu_state, cpu_state->pc);

    // Decode the opcode, 7-bit function code, and registers
    opcode_t opcode = instr & 0x7F;
    funct7_t funct7 = (instr >> 25) & 0x7F;
    riscv_isa_reg_t rs1 = (instr >> 15) & 0x1F;
    riscv_isa_reg_t rs2 = (instr >> 20) & 0x1F;
    riscv_isa_reg_t rd = (instr >> 7) & 0x1F;

    /* Decode the instruction as an I-type instruction, sign extending the
     * immediate value. */
    itype_int_funct3_t itype_funct3 = (instr >> 12) & 0x7;
    int32_t itype_imm = ((int32_t)instr) >> 20;

    // Decode the instruction as an R-type instruction
    rtype_funct3_t rtype_funct3 = (instr >> 12) & 0x7;

    // Decode the 12-bit function code for system instructions
    itype_funct12_t sys_funct12 = (instr >> 20) & 0xFFF;

    switch (opcode)
    {
        // General R-Type arithmetic operation
        case OP_OP: {
            switch (rtype_funct3)
            {
                // 3-bit function code for add or subtract
                case FUNCT3_ADD_SUB: {
                    switch (funct7)
                    {
                        // 7-bit function code for typical integer instructions
                        case FUNCT7_INT: {
                            uint32_t sum = register_read(cpu_state, rs1) +
                                    register_read(cpu_state, rs2);
                            register_write(cpu_state, rd, sum);
                            cpu_state->pc = cpu_state->pc + sizeof(instr);
                            break;
                        }

                        default: {
                            fprintf(stderr, "Encountered unknown/unimplemented "
                                    "7-bit function code 0x%01x. Halting "
                                    "simulation.\n", funct7);
                            cpu_state->halted = true;
                            break;
                        }
                    }
                    break;
                }

                default: {
                    fprintf(stderr, "Encountered unknown/unimplemented "
                            "3-bit rtype function code 0x%01x. Halting "
                            "simulation.\n",
                            rtype_funct3);
                    cpu_state->halted = true;
                    break;
                }
            }
            break;
        }

        // General I-type arithmetic operation
        case OP_IMM: {
            switch (itype_funct3)
            {
                // 3-bit function code for ADDI
                case FUNCT3_ADDI: {
                    uint32_t sum = register_read(cpu_state, rs1) + itype_imm;
                    register_write(cpu_state, rd, sum);
                    cpu_state->pc = cpu_state->pc + sizeof(instr);
                    break;
                }

                default: {
                    fprintf(stderr, "Encountered unknown/unimplemented 3-bit "
                            "itype function code 0x%01x. Halting simulation.\n",
                            rtype_funct3);
                    cpu_state->halted = true;
                    break;
                }
            }
            break;
        }

        // General system operation
        case OP_SYSTEM: {
            switch (sys_funct12)
            {
                // 12-bit function code for ECALL
                case FUNCT12_ECALL: {
                    /* FIXME: To actually support syscalls, the argument
                     * register a0 must be read from the register value, and its
                     * value must equal 0xa to terminate simulation. */
                    uint32_t a0_value = ECALL_ARG_HALT;
                    if (a0_value == ECALL_ARG_HALT) {
                        fprintf(stdout, "ECALL invoked with halt argument, "
                                "halting the simulator.\n");
                        cpu_state->halted = true;
                    }
                    break;
                }

                default: {
                    fprintf(stderr, "Encountered unknown/unimplemented 12-bit "
                            "system function code 0x%03x. Halting "
                            "simulation.\n", sys_funct12);
                    cpu_state->halted = true;
                    break;
                }
            }
            break;
        }

        default: {
            fprintf(stderr, "Encountered unknown opcode 0x%02x. Halting "
                    "simulation.\n", opcode);
            cpu_state->halted = true;
            break;
        }
    }

    return;
}
