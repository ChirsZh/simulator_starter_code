/**
 * sim.c
 *
 * RISC-V 32-bit Instruction Level Simulator.
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

#include <stdio.h>          // Printf and related functions

#include "memory.h"         // Interface to the processor memory
#include "riscv_isa.h"      // Definition of RISC-V opcodes
#include "riscv_abi.h"      // ABI registers and definitions
#include "sim.h"            // Defintions for the simulator

/**
 * process_instruction
 *
 * This is the core part of the simulator. This simulates one instruction, the
 * one currently pointed to by the PC, and updates the next PC state
 * appropiately.
 **/
void process_instruction(cpu_state_t *cpu_state)
{
    // Fetch the 4-bytes for the current instruction
    uint32_t instr = mem_read32(cpu_state, cpu_state->pc);

    // Decode the opcode and registers from the instruction
    opcode_t opcode = instr & 0x7F;
    uint32_t rs1 = (instr >> 15) & 0x1F;
    uint32_t rs2 = (instr >> 20) & 0x1F;
    uint32_t rd = (instr >> 7) & 0x1F;

    /* Decode the instruction as an I-type instruction, sign extending the
     * immediate value. */
    itype_int_funct3_t itype_funct3 = (instr >> 12) & 0x7;
    int32_t itype_imm = ((int32_t)instr) >> 20;

    // Decode the instruction as an R-type instruction
    rtype_int_funct3_t rtype_funct3 = (instr >> 12) & 0x7;
    rtype_arith_funct7_t rtype_funct7 = (instr >> 25) & 0x7F;

    // Decode the 12-bit function code for system instructions
    sys_funct12_t sys_funct12 = (instr >> 20) & 0xFFF;

    switch (opcode)
    {
        // General R-Type arithmetic operation
        case OP_OP:
            switch (rtype_funct3)
            {
                // 3-bit function code for either add or substract
                case FUNCT3_ADD_SUB:
                    switch (rtype_funct7)
                    {
                        // 7-bit function code for ADD
                        case FUNCT7_ADD:
                            if (rd != 0) {
                                cpu_state->regs[rd] = cpu_state->regs[rs1]
                                                      + cpu_state->regs[rs2];
                            }
                            cpu_state->pc = cpu_state->pc + sizeof(instr);
                            break;

                        default:
                            fprintf(stderr, "Encountered unknown/unimplemented "
                                    "7-bit rtype function code 0x%02x. Ending "
                                    "Simulation.\n", rtype_funct7);
                            cpu_state->halted = true;
                            break;
                    }
                    break;

                default:
                    fprintf(stderr, "Encountered unknown/unimplemented 3-bit "
                            "rtype function code 0x%01x. Ending Simulation.\n",
                            rtype_funct3);
                    cpu_state->halted = true;
                    break;
            }
            break;

        // General I-type arithmetic operation
        case OP_IMM:
            switch (itype_funct3)
            {
                // 3-bit function code for ADDI
                case FUNCT3_ADDI:
                    if (rd != 0) {
                        cpu_state->regs[rd] = cpu_state->regs[rs1] +
                                              itype_imm;
                    }
                    cpu_state->pc = cpu_state->pc + sizeof(instr);
                    break;

                default:
                    fprintf(stderr, "Encountered unknown/unimplemented 3-bit "
                            "itype function code 0x%01x. Ending Simulation.\n",
                            rtype_funct3);
                    cpu_state->halted = true;
                    break;
            }
            break;

        // General system operation
        case OP_SYSTEM:
            switch (sys_funct12)
            {
                // 12-bit function code for ECALL
                case FUNCT12_ECALL:
                    if (cpu_state->regs[REG_A0] == ECALL_ARG_HALT) {
                        fprintf(stdout, "ECALL invoked with halt argument, "
                                "halting the simulator.\n");
                        cpu_state->halted = true;
                    }
                    break;

                default:
                    fprintf(stderr, "Encountered unknown/unimplemented 12-bit "
                            "system function code 0x%03x. Ending simulation.\n",
                            sys_funct12);
                    cpu_state->halted = true;
                    break;
            }
            break;

        default:
            fprintf(stderr, "Encountered unknown/unimplemented opcode 0x%02x. "
                    "Ending Simulation.\n", opcode);
            cpu_state->halted = true;
            break;
    }

    return;
}
