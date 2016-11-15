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

#include "commands.h"       // This file's interface

/*----------------------------------------------------------------------------
 * Step and Go Commands
 *----------------------------------------------------------------------------*/

/**
 * command_step
 *
 * Runs a the simulator for a specified number of cycles or until the processor
 * is halted. The user can optionally specify the number of cycles, otherwise
 * the default is one.
 **/
void command_step(cpu_state_t *cpu_state, const char *args[], int num_args)
{
    // Silence the compiler
    (void)cpu_state;
    (void)args;
    (void)num_args;

    return;
}

/**
 * command_go
 *
 * Runs the simulator until program completion or an exception is encountered.
 **/
void command_go(cpu_state_t *cpu_state, const char *args[], int num_args)
{
    // Silence the compiler
    (void)cpu_state;
    (void)args;
    (void)num_args;

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
