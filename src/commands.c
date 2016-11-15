/**
 * @file commands.c
 * @date Sunday, November 13, 2016 at 09:10:00 PM EST
 * @author Brandon Perez (bmperez)
 *
 * This file contains the implementation for the shell commands.
 *
 * The commands are how the user interacts with the simulator from the shell.
 * The commands are pretty basic, such as stepping the program, displaying
 * registers, etc.
 *
 * @bug No known bugs.
 **/

#include <stdlib.h>         // Malloc and related functions
#include <stdio.h>          // Printf and related functions
#include <stdint.h>         // Fixed-size integral types

#include <errno.h>          // Error codes and perror
#include <limits.h>         // Limits for integer types

#include "sim.h"            // Interface
#include "commands.h"       // This file's interface

/*----------------------------------------------------------------------------
 * Parsing Helper Functions
 *----------------------------------------------------------------------------*/

static int parse_int(const char *arg_str, int *val)
{
    // Attempt to parse the string as a signed long
    errno = 0;
    long parsed_val = strtol(arg_str, NULL, 10);
    if (errno != 0) {
        return -errno;
    } else if (parsed_val < INT_MIN || parsed_val > INT_MAX) {
        return -ERANGE;
    }

    // If we could parse the value, then cast it to an integer
    *val = (int)parsed_val;
    return 0;
}


/*----------------------------------------------------------------------------
 * Step and Go Commands
 *----------------------------------------------------------------------------*/

// The maximum number of arguments that can be specified to the step command
#define STEP_MAX_NUM_ARGS   1

// The expected number of arguments for the go command
#define GO_NUM_ARGS         0

/**
 * run_simulator
 *
 * Run the simulator for a single cycle, incrementing the instruction count.
 **/
static void run_simulator(cpu_state_t *cpu_state)
{
    process_instruction(cpu_state);
    cpu_state->instr_count += 1;
    return;
}

/**
 * command_step
 *
 * Runs a the simulator for a specified number of cycles or until the processor
 * is halted. The user can optionally specify the number of cycles, otherwise
 * the default is one.
 **/
void command_step(cpu_state_t *cpu_state, const char *args[], int num_args)
{
    // Check that the appropiate number of arguments was specified
    if (num_args > STEP_MAX_NUM_ARGS) {
        fprintf(stderr, "Error: Too many arguments specified to 'step' "
                "command.\n");
        return;
    }

    // If a number of cycles was specified, then attempt to parse it
    int num_cycles = 1;
    if (num_args != 0 && parse_int(args[0], &num_cycles) < 0) {
        fprintf(stderr, "Error: Unable to parse '%s' as an int.\n", args[0]);
        return;
    }

    // If the processor is halted, then we don't do anything.
    if (cpu_state->halted) {
        fprintf(stdout, "Processor is halted, cannot run the simulator.\n");
        return;
    }

    /* Run the simulator for the specified number of cycles, or until the
     * processor is halted. */
    for (int i = 0; i < num_cycles && !cpu_state->halted; i++)
    {
        run_simulator(cpu_state);
    }

    return;
}

/**
 * command_go
 *
 * Runs the simulator until program completion or an exception is encountered.
 **/
void command_go(cpu_state_t *cpu_state, const char *args[], int num_args)
{
    // Silence unused variable warnings from the compiler
    (void)args;

    // Check that the appropiate number of arguments was specified
    if (num_args != GO_NUM_ARGS) {
        fprintf(stderr, "Error: Improper number of arguments specified to "
                "'go' command.\n");
        return;
    }

    // If the processor is halted, then we don't do anything.
    if (cpu_state->halted) {
        fprintf(stdout, "Processor is halted, cannot run the simulator.\n");
        return;
    }

    // Run the simulator until the processor is halted
    while (!cpu_state->halted)
    {
        run_simulator(cpu_state);
    }

    return;
}

/*----------------------------------------------------------------------------
 * Reg and Rdump Commands
 *----------------------------------------------------------------------------*/

/**
 * command_reg
 *
 * Display the value of the specified register to the user. The user can
 * optionally specify a value to update the register's value instead.
 **/
void command_reg(cpu_state_t *cpu_state, const char *args[], int num_args)
{
    // Silence the compiler
    (void)cpu_state;
    (void)args;
    (void)num_args;

    return;
}

/**
 * comand_rdump
 *
 * Displays the value of all registers in the system, along with the number of
 * instructions executed so far. The user can optionally specify a file to dump
 * the values to.
 **/
void command_rdump(cpu_state_t *cpu_state, const char *args[], int num_args)
{
    // Silence the compiler
    (void)cpu_state;
    (void)args;
    (void)num_args;

    return;
}

/*----------------------------------------------------------------------------
 * Memory and Mdump Commands
 *----------------------------------------------------------------------------*/

/**
 * command_memory
 *
 * Displays the value of the specified memory address to the user. The user can
 * optionally specify a value to update the memory locations value instead.
 **/
void command_memory(cpu_state_t *cpu_state, const char *args[], int num_args)
{
    // Silence the compiler
    (void)cpu_state;
    (void)args;
    (void)num_args;

    return;
}

/**
 * command_mdump
 *
 * Displays the values of a range of memory locations in the system. The user
 * can optionally specify a file to dump the memory values to.
 **/
void command_mdump(cpu_state_t *cpu_state, const char *args[], int num_args)
{
    // Silence the compiler
    (void)cpu_state;
    (void)args;
    (void)num_args;

    return;
}

/*----------------------------------------------------------------------------
 * Restart and Load Commands
 *----------------------------------------------------------------------------*/

/**
 * command_restart
 *
 * Resets the processor and restarts the currently loaded program from its first
 * instruction.
 **/
void command_restart(cpu_state_t *cpu_state, const char *args[], int num_args)
{
    // Silence the compiler
    (void)cpu_state;
    (void)args;
    (void)num_args;

    return;
}

/**
 * command_load
 *
 * Resets the processor and loads a new program into the processor, replacing
 * the currently executing program. The execution starts from the beginning of
 * the loaded program.
 **/
void command_load(cpu_state_t *cpu_state, const char *args[], int num_args)
{
    // Silence the compiler
    (void)cpu_state;
    (void)args;
    (void)num_args;

    return;
}

/*----------------------------------------------------------------------------
 * Help and Quit Commands
 *----------------------------------------------------------------------------*/

/**
 * command_quit
 *
 * Quits the simulator.
 **/
void command_quit(cpu_state_t *cpu_state, const char *args[], int num_args)
{
    // Silence the compiler
    (void)cpu_state;
    (void)args;
    (void)num_args;

    return;
}

/**
 * command_help
 *
 * Displays a help message to the user, explaining the commands in for the
 * simulator and how to use them.
 **/
void command_help(cpu_state_t *cpu_state, const char *args[], int num_args)
{
    // Silence the compiler
    (void)cpu_state;
    (void)args;
    (void)num_args;

    return;
}
