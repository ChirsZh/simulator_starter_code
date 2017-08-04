/**
 * commands.h
 *
 * RISC-V 32-bit Instruction Level Simulator
 *
 * ECE 18-447
 * Carnegie Mellon University
 *
 * This file contains the interface to the shell commands.
 *
 * This is the interface to implementation of all the commands available in the
 * shell.
 *
 * Authors:
 *  - 2016 - 2017: Brandon Perez
 **/

/*----------------------------------------------------------------------------*
 *                          DO NOT MODIFY THIS FILE!                          *
 *          You should only add or change files in the src directory!         *
 *----------------------------------------------------------------------------*/

#ifndef COMMANDS_H_
#define COMMANDS_H_

// Standard Includes
#include <stdbool.h>            // Definition of the boolean type

// 18-447 Simulator Includes
#include <sim.h>                // Definition of cpu_state_t

/*----------------------------------------------------------------------------
 * Definitions
 *----------------------------------------------------------------------------*/

// Indicates that a SIGINT signal was received by the program
volatile bool SIGINT_RECEIVED;

/*----------------------------------------------------------------------------
 * CPU Initialization
 *----------------------------------------------------------------------------*/

/**
 * Initializes the CPU state.
 *
 * This loads the specified program into the processor's memory, clearing out
 * any prior loaded program. Additionally, the register values are reset to
 * their proper starting values.
 **/
int init_cpu_state(cpu_state_t *cpu_state, char *program_path);

/*----------------------------------------------------------------------------
 * Commands
 *----------------------------------------------------------------------------*/

/**
 * Runs the simulator for a specified number of cycles or until a halt.
 *
 * The user can optionally specify the number of cycles. Otherwise, the default
 * is to run the processor one cycle. If the processor is halted before the
 * number of steps is reached, then simulation stops.
 **/
void command_step(cpu_state_t *cpu_state, char *args[], int num_args);

/**
 * Runs the simulator until program completion or an exception is encountered.
 *
 * In the case of an infinite running program because of a bug in the
 * implementation, the user can interrupt execution with a keyboard interrupt.
 **/
void command_go(cpu_state_t *cpu_state, char *args[], int num_args);

/**
 * Display the value of the specified register to the user.
 *
 * The user can optionally specify a value, in which case, the command
 * updates the register's value instead of reading it.
 **/
void command_reg(cpu_state_t *cpu_state, char *args[], int num_args);

/**
 * Displays the value of the specified memory address to the user.
 *
 * The user can optionally specify a value, in which case, the command updates
 * the memory location's value instead of reading it.
 **/
void command_mem(cpu_state_t *cpu_state, char *args[], int num_args);

/**
 * Displays the value of all the CPU registers, along with other information.
 *
 * The PC value and number of instructions executed so far are also displayed
 * with the register values. The user can optionally specify a file to which to
 * dump the register values.
 **/
void command_rdump(cpu_state_t *cpu_state, char *args[], int num_args);

/**
 * Displays the values of a range of memory locations in the system.
 *
 * The user can optionally specify a file to which to dump the memory values.
 **/
void command_mdump(cpu_state_t *cpu_state, char *args[], int num_args);

/**
 * Resets the processor and restarts the currently loaded program.
 *
 * The program is naturally started again from its first instruction, with all
 * register and memory values reset to their starting values.
 **/
void command_restart(cpu_state_t *cpu_state, char *args[], int num_args);

/**
 * Loads a new program into the processor's memory to be executed.
 *
 * This resets the processor and loads a new program into the processor,
 * replacing the currently executing program. The execution starts from the
 * beginning of the loaded program, with all memory and register values at their
 * proper starting values.
 **/
void command_load(cpu_state_t *cpu_state, char *args[], int num_args);

/**
 * Toggles verbose mode for the simulator.
 *
 * If verbose mode is active, then the simulator performs a register dump after
 * every cycle that the CPU runs. This can be useful to do a cycle-by-cycle
 * diff between a reference implementation and this implementation.
 **/
void command_verbose(cpu_state_t *cpu_state, char *args[], int num_args);

/**
 * Quits the simulator.
 **/
bool command_quit(cpu_state_t *cpu_state, char *args[], int num_args);

/**
 * Displays a help message to the user.
 *
 * The message explains the commands available in the simulator and how to use
 * them.
 **/
void command_help(cpu_state_t *cpu_state, char *args[], int num_args);

#endif /* COMMANDS_H_ */
