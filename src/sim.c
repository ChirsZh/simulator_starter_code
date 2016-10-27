/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   ECE 447                                                   */
/*   Carnegie Mellon University                                */
/*                                                             */
/***************************************************************/

/* Lab 1.a Spring 2016
 * Complete the execute() function below by adding cases
 * for the missing opcodes and subocodes (see mips.h). 
 *
 * **NOTE*** You are only allowed to make modifications in the 
 * three specifically designated regions.
 */

#include <stdio.h>
#include "shell.h"
#include "mips.h"

uint32_t dcd_op;     /* decoded opcode */
uint32_t dcd_rs;     /* decoded rs operand */
uint32_t dcd_rt;     /* decoded rt operand */
uint32_t dcd_rd;     /* decoded rd operand */
uint32_t dcd_shamt;  /* decoded shift amount */
uint32_t dcd_funct;  /* decoded function */
uint32_t dcd_imm;    /* decoded immediate value */
uint32_t dcd_target; /* decoded target address */
int      dcd_se_imm; /* decoded sign-extended immediate value */
uint32_t inst;       /* machine instruction */


uint32_t sign_extend_b2w(uint8_t c)
{
  return (c & 0x80) ? (c | 0xffffff80) : c;
}

uint32_t sign_extend_h2w(uint16_t c)
{
  return (c & 0x8000) ? (c | 0xffff8000) : c;
}

/****************************************************************/
/*** you may add your own auxiliary functions below this line ***/






/*** you may add your own auxiliary functions above this line ***/
/****************************************************************/

void process_instruction()
{
  /*  function: process_instruction
   *  
   *    Process one instruction at a time  
   */

  /* fetch the 4 bytes of the current instruction */
  inst = mem_read_32(CURRENT_STATE.PC);

  /* decode an instruction into fields */
  dcd_op     = (inst >> 26) & 0x3F;
  dcd_rs     = (inst >> 21) & 0x1F;
  dcd_rt     = (inst >> 16) & 0x1F;
  dcd_rd     = (inst >> 11) & 0x1F;
  dcd_shamt  = (inst >> 6) & 0x1F;
  dcd_funct  = (inst >> 0) & 0x3F;
  dcd_imm    = (inst >> 0) & 0xFFFF;
  dcd_se_imm = sign_extend_h2w(dcd_imm);
  dcd_target = (inst >> 0) & ((1UL << 26) - 1);

  /* update architectural state according to instruction */
  switch (dcd_op) {
  case OP_SPECIAL: 
    { 
      switch (dcd_funct) {
      case SUBOP_ADD: 
      case SUBOP_ADDU: 
	if (dcd_rd!=0)
	  NEXT_STATE.REGS[dcd_rd] = CURRENT_STATE.REGS[dcd_rs] + CURRENT_STATE.REGS[dcd_rt];
	NEXT_STATE.PC = CURRENT_STATE.PC + 4;
	break;
      case SUBOP_SYSCALL:
	if (CURRENT_STATE.REGS[2] == 10)
	  RUN_BIT = 0;
	NEXT_STATE.PC = CURRENT_STATE.PC + 4;
	break;
/*************************************************************/
/*** specify the remaining dcd_funct cases below this line ***/
	
	
	
	
	
/*** specify the remaining dcd_funct cases above this line ***/
/*************************************************************/
      default:
	printf("Encountered unimplemented subopcode (0x%x). Ending simulation...\n\n", dcd_funct);
	RUN_BIT = 0;
      } /* switch (dcd_funct) */
    } /* case OP_SPECIAL */
    break;
  case OP_ADDI:
  case OP_ADDIU:
    if (dcd_rt!=0)
      NEXT_STATE.REGS[dcd_rt] = CURRENT_STATE.REGS[dcd_rs] + dcd_se_imm;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    break;
/**********************************************************/
/*** specify the remaining dcd_op cases below this line ***/





/*** specify the remaining dcd_op cases above this line ***/
/*********************************************************/
  default:
    printf("Encountered unimplemented opcode (0x%x). Ending simulation...\n\n", dcd_op);
    RUN_BIT = 0;
  } /* switch (dcd_op) */
}
