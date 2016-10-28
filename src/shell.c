/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   ECE 447                                                   */
/*   Carnegie Mellon University                                */
/*                                                             */
/***************************************************************/

/*----------------------------------------------------------------------------*
 *                          DO NOT MODIFY THIS FILE!                          *
 *                 You should only add files or change sim.c!                 *
 *----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>          // Printf and related functions
#include <stdint.h>         // Fixed-size integral types

#include <assert.h>         // Assert macro
#include <string.h>         // String manipulation functions

#include "shell.h"          // Interface to the core simulator
#include "memory.h"         // Interface to the processor memory

// FIXME: Temporary
#define USER_TEXT_START     0x00400000

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {
  printf("----------------MIPS ISIM Help-----------------------\n");
  printf("go               -  run program to completion         \n");
  printf("run n            -  execute program for n instructions\n");
  printf("mdump low high   -  dump memory from low to high      \n");
  printf("rdump            -  dump the register & bus values    \n");
  printf("input reg_no reg_value - set GPR reg_no to reg_value  \n");
  printf("high value             - set the HI register to value \n");
  printf("low value              - set the LO register to value \n");
  printf("?                -  display this help menu            \n");
  printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle(cpu_state_t *cpu_state) {
  process_instruction(cpu_state);
  cpu_state->instr_count += 1;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate MIPS for n cycles                      */
/*                                                             */
/***************************************************************/
void run(cpu_state_t *cpu_state, int num_cycles) {
  int i;

  if (!cpu_state->running) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating for %d cycles...\n\n", num_cycles);
  for (i = 0; i < num_cycles; i++) {
    if (!cpu_state->running) {
	    printf("Simulator halted\n\n");
	    break;
    }
    cycle(cpu_state);
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate MIPS until HALTed                      */
/*                                                             */
/***************************************************************/
void go(cpu_state_t *cpu_state) {
  if (!cpu_state->running) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating...\n\n");
  while (cpu_state->running)
    cycle(cpu_state);
  printf("Simulator halted\n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE* dumpsim_file, int start, int stop) {
  int address;

  printf("\nMemory content [0x%08x..0x%08x] :\n", start, stop);
  printf("-------------------------------------\n");
  for (address = start; address <= stop; address += 4)
    printf("  0x%08x (%d) : 0x%08x\n", address, address, mem_read32(address));
  printf("\n");

  /* dump the memory contents into the dumpsim file */
  fprintf(dumpsim_file, "\nMemory content [0x%08x..0x%08x] :\n", start, stop);
  fprintf(dumpsim_file, "-------------------------------------\n");
  for (address = start; address <= stop; address += 4)
    fprintf(dumpsim_file, "  0x%08x (%d) : 0x%08x\n", address, address, mem_read32(address));
  fprintf(dumpsim_file, "\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(const cpu_state_t *cpu_state, FILE * dumpsim_file) {
  int k;

  printf("\nCurrent register/bus values :\n");
  printf("-------------------------------------\n");
  printf("Instruction Count : %u\n", cpu_state->instr_count);
  printf("PC                : 0x%08x\n", cpu_state->pc);
  printf("Registers:\n");
  for (k = 0; k < RISCV_NUM_REGS; k++)
    printf("R%d: 0x%08x\n", k, cpu_state->regs[k]);
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
  fprintf(dumpsim_file, "-------------------------------------\n");
  fprintf(dumpsim_file, "Instruction Count : %u\n", cpu_state->instr_count);
  fprintf(dumpsim_file, "PC                : 0x%08x\n", cpu_state->pc);
  fprintf(dumpsim_file, "Registers:\n");
  for (k = 0; k < RISCV_NUM_REGS; k++)
    fprintf(dumpsim_file, "R%d: 0x%08x\n", k, cpu_state->regs[k]);
  fprintf(dumpsim_file, "\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */
/*                                                             */
/***************************************************************/
void get_command(cpu_state_t *cpu_state, FILE* dumpsim_file) {
  char buffer[20];
  int start, stop, cycles;
  int register_no, register_value;

  printf("RISCV-SIM> ");

  if (scanf("%s", buffer) == EOF)
      exit(0);

  printf("\n");

  switch(buffer[0]) {
  case 'G':
  case 'g':
    go(cpu_state);
    break;

  case 'M':
  case 'm':
    if (scanf("%i %i", &start, &stop) != 2)
        break;

    mdump(dumpsim_file, start, stop);
    break;

  case '?':
    help();
    break;

  case 'Q':
  case 'q':
    printf("Bye.\n");
    exit(0);

  case 'R':
  case 'r':
    if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(cpu_state, dumpsim_file);
    else {
	    if (scanf("%d", &cycles) != 1) break;
	    run(cpu_state, cycles);
    }
    break;

  case 'I':
  case 'i':
   if (scanf("%i %i", &register_no, &register_value) != 2)
      break;
   cpu_state->regs[register_no] = register_value;
   break;

  default:
    printf("Invalid Command\n");
    break;
  }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(cpu_state_t *cpu_state, char *program_filename) {
  FILE * prog;
  int ii, word;

  /* Open program file. */
  prog = fopen(program_filename, "r");
  if (prog == NULL) {
    printf("Error: Can't open program file %s\n", program_filename);
    exit(-1);
  }

  /* Read in the program. */

  ii = 0;
  while (fscanf(prog, "%x\n", &word) != EOF) {
    mem_write32(USER_TEXT_START + ii, word);
    ii += 4;
  }

  cpu_state->pc = USER_TEXT_START;

  printf("Read %d words from program into memory.\n\n", ii/4);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(cpu_state_t *cpu_state, char *program_filename,
        int num_prog_files) {
  int i;

  mem_init();
  for ( i = 0; i < num_prog_files; i++ ) {
    load_program(cpu_state, program_filename);
    while(*program_filename++ != '\0');
  }

  cpu_state->running = true;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {
  cpu_state_t cpu_state;
  FILE * dumpsim_file;

  /* Error Checking */
  if (argc < 2) {
    printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
           argv[0]);
    exit(1);
  }

  printf("MIPS Simulator\n\n");

  initialize(&cpu_state, argv[1], argc - 1);

  if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
    printf("Error: Can't open dumpsim file\n");
    exit(-1);
  }

  while (1)
    get_command(&cpu_state, dumpsim_file);

}
