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
#include <errno.h>          // Error codes and perror

#include "sim.h"            // Interface to the core simulator, cpu_state_t
#include "memory.h"         // Interface to the processor memory
#include "commands.h"       // Interface to the shell commands

/* The maximum number of command line arguments that can be specified, including
 * the program name. */
const int MAX_CMDLINE_ARGS  = 2;

// The maximum line length the user can type in for a command
#define COMMAND_MAX_LEN     100

/* The maximum number of arguments that can be parsed from user input. This more
 * than the max possible, so too many arguments can be detected. */
#define COMMAND_MAX_ARGS    4

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

/*----------------------------------------------------------------------------
 * Command Line Parsing
 *----------------------------------------------------------------------------*/

/**
 * print_usage
 *
 * Prints the usage message for the program.
 **/
static void print_usage()
{
    fprintf(stdout, "Usage: riscv-sim [program]\n");
    return;
}

/**
 * parse_arguments
 *
 * Parses the command-line arguments to the program, which only consist of an
 * optional path to the program to run.
 **/
static int parse_arguments(int argc, char *argv[], char **program_path)
{
    // Check that the proper number of command line arguments was specified
    if (argc > MAX_CMDLINE_ARGS) {
        fprintf(stderr, "Error: Too many command line arguments specified.\n");
        print_usage();
        return -EINVAL;
    }

    // If it was specified, parse the program path from the command line
    if (argc > 1) {
        *program_path = argv[1];
    } else {
        *program_path = NULL;
    }

    return 0;
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
    const char *args[], int num_args, bool *quit)
{
    // Assume the command is not quit
    *quit = false;

    // Run the appropiate command based on what the user specified
    if (strcmp(command, "step") == 0) {
        command_step(cpu_state, args, num_args);
    } else if (strcmp(command, "go") == 0) {
        command_go(cpu_state, args, num_args);
    } else if (strcmp(command, "reg") == 0) {
        command_reg(cpu_state, args, num_args);
    } else if (strcmp(command, "memory") == 0) {
        command_memory(cpu_state, args, num_args);
    } else if (strcmp(command, "rdump") == 0) {
        command_rdump(cpu_state, args, num_args);
    } else if (strcmp(command, "mdump") == 0) {
        command_mdump(cpu_state, args, num_args);
    } else if (strcmp(command, "restart") == 0) {
        command_restart(cpu_state, args, num_args);
    } else if (strcmp(command, "load") == 0) {
        command_load(cpu_state, args, num_args);
    } else if (strcmp(command, "quit") == 0) {
        *quit = true;
        command_quit(cpu_state, args, num_args);
    } else if (strcmp(command, "help") == 0) {
        command_help(cpu_state, args, num_args);
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
 *
 * As this relies on aliases, this function should be called after
 * process_long_command, or it may parse commands incorrectly.
 **/
static bool process_short_command(cpu_state_t *cpu_state, const char *command,
        const char *args[], int num_args, bool *quit)
{
    // Assume the command is not quit
    *quit = false;

    // Based on the first character, run the appropiate command
    switch (command[0]) {
        case 's':
            command_step(cpu_state, args, num_args);
            return true;

        case 'g':
            command_go(cpu_state, args, num_args);
            return true;

        case 'r':
            command_reg(cpu_state, args, num_args);
            return true;

        case 'm':
            command_memory(cpu_state, args, num_args);
            return true;

        case 'q':
            command_quit(cpu_state, args, num_args);
            *quit = true;
            return true;

        case '?':
        case 'h':
            command_help(cpu_state, args, num_args);
            return true;
    }

    return false;
}

/**
 * Splits the user input command string into a command and argument list.
 *
 * The command returned is a typical null-terminated string, while the argument
 * list is terminated by a NULL pointer. The length of the argument list is
 * returned returned.
 **/
static int split_command(char *command_string, char **command,
        const char *args[COMMAND_MAX_ARGS+1])
{
    /* First, parse the command out of the string. If the string is empty, then
     * there aren't any arguments to parse. */
    char *string_tail;
    *command = strtok_r(command_string, " ", &string_tail);
    if (*command == NULL) {
        return 0;
    }

    // Tokenize each word in the string, and add it to the argument array
    int num_args = 0;
    char *word;
    while (num_args < COMMAND_MAX_ARGS)
    {
        // Parse and tokenize the next word from the string
        word = strtok_r(NULL, " ", &string_tail);
        if (word == NULL) {
            break;
        }

        // We were able to parse another word, so add it to the array
        args[num_args] = word;
        num_args += 1;
    }

    // Terminate the array with a NULL pointer, return number of arguments
    args[num_args] = NULL;
    return num_args;
}

/**
 * process_command
 *
 * Attempts to parse and process the command specified by the user as either the
 * long form of the command, a string), or the short form, a single character.
 * Returns true if the quit command was specified.
 **/
static bool process_command(cpu_state_t *cpu_state, char *command_string)
{
    // Seperate the command string into the command and a list of arguments
    char *command;
    const char *args[COMMAND_MAX_ARGS+1];
    int num_args = split_command(command_string, &command, args);

    // The user entered an empty line, so there's no command to process
    if (command == NULL) {
        return true;
    }

    // Otherwise, identify the command based on its short alias or long form
    bool quit;
    if (process_long_command(cpu_state, command, args, num_args, &quit)) {
        return quit;
    } else if (process_short_command(cpu_state, command, args, num_args,
                &quit)) {
        return quit;
    } else {
        fprintf(stderr, "Error: Invalid command '%s' specified.\n", command);
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
        char line[COMMAND_MAX_LEN+1];
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
static int init_cpu_state(cpu_state_t *cpu_state, char *program_path)
{
    // Clear out the CPU state, and Initialize the CPU state fields
    cpu_state->instr_count = 0;
    memset(cpu_state->regs, 0, sizeof(cpu_state->regs));

    // If a program was specified, then load it into the processor memory
    if (program_path == NULL) {
        cpu_state->halted = true;
        return 0;
    } else {
        int rc = mem_load_program(cpu_state, program_path);
        cpu_state->halted = (rc != 0);
        return rc;
    }
}

/**
 * main
 *
 * The main method for the simulator. This parses the command line arguments,
 * initializes the CPU and starts up the REPL for the simulator.
 **/
int main(int argc, char *argv[])
{
    // Parse the program filename from the command line
    char *program_path;
    int rc = parse_arguments(argc, argv, &program_path);
    if (rc < 0) {
        return -rc;
    }

    // Initialize the CPU state, loading the program if specified
    cpu_state_t cpu_state;
    rc = init_cpu_state(&cpu_state, program_path);
    if (rc < 0) {
        return -rc;
    }

    // The REPL loop for the simulator, wait for and read user commands
    simulator_repl(&cpu_state);
    return 0;
}
