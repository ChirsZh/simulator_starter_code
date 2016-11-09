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
#include <stdio.h>          // Printf, fgets, and related functions
#include <stdint.h>         // Fixed-size integral types

#include <assert.h>         // Assert macro
#include <string.h>         // String manipulation functions and memset

#include "sim.h"            // Interface to the core simulator
#include "memory.h"         // Interface to the processor memory

// The maximum line length the user can type in
#define MAX_LINE_LEN        100

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
void mdump(cpu_state_t *cpu_state, FILE* dumpsim_file, int start,
        int stop) {
  int address;

  printf("\nMemory content [0x%08x..0x%08x] :\n", start, stop);
  printf("-------------------------------------\n");
  for (address = start; address <= stop; address += 4)
    printf("  0x%08x (%d) : 0x%08x\n", address, address, mem_read32(cpu_state, address));
  printf("\n");

  /* dump the memory contents into the dumpsim file */
  fprintf(dumpsim_file, "\nMemory content [0x%08x..0x%08x] :\n", start, stop);
  fprintf(dumpsim_file, "-------------------------------------\n");
  for (address = start; address <= stop; address += 4)
    fprintf(dumpsim_file, "  0x%08x (%d) : 0x%08x\n", address, address, mem_read32(cpu_state, address));
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

    mdump(cpu_state, dumpsim_file, start, stop);
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
    mem_write32(cpu_state, USER_TEXT_START + ii, word);
    ii += 4;
  }

  cpu_state->pc = USER_TEXT_START;

  printf("Read %d words from program into memory.\n\n", ii/4);
}

/*----------------------------------------------------------------------------
 * Simulator REPL
 *----------------------------------------------------------------------------*/

/**
 * process_long_command
 *
 * Attempts to parse and process the command as one of its string variants.
 * If the command matches one of the simulator's commands, it is executed.
 * Returns true if the string matched a command. Sets the quit boolean pointer
 * if a quit command was specified.
 **/
static bool process_long_command(cpu_state_t *cpu_state, const char *command,
    bool *quit)
{
    // Assume the command is not quit
    *quit = false;

    // Run the appropiate command based on what the user specified
    if (strcmp(command, "step") == 0) {
        process_command_step(cpu_state, command);
    } else if (strcmp(command, "go") == 0) {
        process_command_go(cpu_state);
    } else if (strcmp(command, "reg") == 0) {
        process_command_reg(cpu_state, command);
    } else if (strcmp(command, "memory") == 0) {
        process_command_memory(cpu_state, command);
    } else if (strcmp(command, "rdump") == 0) {
        process_command_rdump(cpu_state, command);
    } else if (strcmp(command, "mdump") == 0) {
        process_command_dump(cpu_state, command);
    } else if (strcmp(command, "restart") == 0) {
        process_command_restart(cpu_state, command);
    } else if (strcmp(command, "load") == 0) {
        process_command_load(cpu_state, command);
    } else if (strcmp(command, "quit") == 0) {
        process_command_quit(cpu_state, command);
    } else {
        return false;
    }

    return true;
}

/**
 * process_short_command
 *
 * Attempts to parse and process the command as one of its single character
 * variants. If the command matches one of the simulator's commands, it is
 * executed. Returns true if the string matched a command. Sets the quit boolean
 * pointer if a quit command was specified.
 **/
static bool process_short_command(cpu_state_t *cpu_state, const char *command,
        bool *quit)
{
    // Assume the command is not quit
    *quit = false;

    // Based on the first character, run the appropiate command
    switch (command[0]) {
        case 's':
            command_step(cpu_state, command);
            return true;

        case 'g':
            command_go(cpu_state);
            return true;

        case 'r':
            command_reg(cpu_state, command);
            return true;

        case 'm':
            command_memory(cpu_state, command);
            return true;

        case 'q':
            command_quit(cpu_state, command, quit);
            return true;

        case '?':
        case 'h':
            command_help(cpu_state, command);
            return true;
    }

    return false;
}

/**
 * process_command
 *
 * Attempts to parse and process the command specified by the user as either the
 * long form of the command, a string), or the short form, a single character.
 **/
static bool process_command(cpu_state_t *cpu_state, const char *command)
{
    bool quit = false;
    if (process_long_command(cpu_state, command, &quit)) {
        return quit;
    } else if (process_short_command(cpu_state, command, &quit)) {
        return quit;
    } else {
        fprintf(stderr, "Invalid command '%s' specified.\n", command);
        return false;
    }
}

/**
 * simulator_repl
 *
 * The read-eval-print loop (REPL) for the simulator. Continuously waits for
 * user input, and performs the specified command, until the user specifies quit
 * or sends an EOF character.
 **/
static void simulator_repl(cpu_state_t *cpu_state)
{
    // Continuously process user commands until a quit or EOF
    while (true)
    {
        // Print the command prompt for the user
        fprintf(stdout, "RISC-V Sim> ");
        fflush(stdout);

        // Read the next line from the user, terminating on an EOF character
        char line[MAX_LINE_LEN + 1];
        char *status = fgets(line, sizeof(line), stdin);
        if (status == NULL) {
            break;
        }

        // Strip the newline character from the input
        int newline_index = strcspn(line, "\r\n");
        line[newline_index] = '\0';

        // Process the user's command, stop if the user requested quit
        bool quit = process_command(cpu_state, line);
        if (quit) {
            break;
        }
    }

    return;
}

/**
 * init_cpu_state
 *
 * Initializes the CPU state, and loads the specified program into the
 * processor's memory.
 **/
static void init_cpu_state(cpu_state_t *cpu_state, char *program_name)
{
    // Clear out the CPU state, and Initialize the CPU state fields
    cpu_state->instr_count = 0;
    memset(cpu_state->regs, 0, sizeof(cpu_state->regs));

    // Initialize the processor memory, and mark the CPU as running
    mem_init(cpu_state);
    cpu_state->running = true;
}

/**
 * main
 *
 * The main method for the simulator. This parses the command line arguments,
 * initializes the CPU and starts up the REPL for the simulator.
 **/
int main(int argc, char *argv[])
{
    // Parse the program filename from the command line, and initialize the CPU
    cpu_state_t cpu_state;
    //char *program_name = parse_arguments(argc, argv);
    char *program_name = NULL;
    init_cpu_state(&cpu_state, program_name);

    // The REPL loop for the simulator, wait for and read user commands
    simulator_repl(&cpu_state);
    return 0;
}
