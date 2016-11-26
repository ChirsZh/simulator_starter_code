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
 *  - 2016: Brandon Perez
 **/

/*----------------------------------------------------------------------------*
 *                          DO NOT MODIFY THIS FILE!                          *
 *          You should only add or change files in the src directory!         *
 *----------------------------------------------------------------------------*/

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "sim.h"            // Definition of cpu_state_t

/*----------------------------------------------------------------------------
 * CPU Initialization
 *----------------------------------------------------------------------------*/

/**
 * init_cpu_state
 *
 * Initializes the CPU state, and loads the specified program into the
 * processor's memory.
 **/
int init_cpu_state(cpu_state_t *cpu_state, char *program_path);

/*----------------------------------------------------------------------------
 * Commands
 *----------------------------------------------------------------------------*/

/**
 * command_step
 *
 * Runs a the simulator for a specified number of cycles or until the processor
 * is halted. The user can optionally specify the number of cycles, otherwise
 * the default is one.
 **/
void command_step(cpu_state_t *cpu_state, char *args[], int num_args);

/**
 * command_go
 *
 * Runs the simulator until program completion or an exception is encountered.
 **/
void command_go(cpu_state_t *cpu_state, char *args[], int num_args);

/**
 * command_reg
 *
 * Display the value of the specified register to the user. The user can
 * optionally specify a value to update the register's value instead.
 **/
void command_reg(cpu_state_t *cpu_state, char *args[], int num_args);

/**
 * command_mem
 *
 * Displays the value of the specified memory address to the user. The user can
 * optionally specify a value to update the memory locations value instead.
 **/
void command_mem(cpu_state_t *cpu_state, char *args[], int num_args);

/**
 * comand_rdump
 *
 * Displays the value of all registers in the system, along with the number of
 * instructions executed so far. The user can optionally specify a file to dump
 * the values to.
 **/
void command_rdump(cpu_state_t *cpu_state, char *args[], int num_args);

/**
 * command_mdump
 *
 * Displays the values of a range of memory locations in the system. The user
 * can optionally specify a file to dump the memory values to.
 **/
void command_mdump(cpu_state_t *cpu_state, char *args[], int num_args);

/**
 * command_restart
 *
 * Resets the processor and restarts the currently loaded program from its first
 * instruction.
 **/
void command_restart(cpu_state_t *cpu_state, char *args[], int num_args);

/**
 * command_load
 *
 * Resets the processor and loads a new program into the processor, replacing
 * the currently executing program. The execution starts from the beginning of
 * the loaded program.
 **/
void command_load(cpu_state_t *cpu_state, char *args[], int num_args);

/**
 * command_quit
 *
 * Quits the simulator.
 **/
bool command_quit(cpu_state_t *cpu_state, char *args[], int num_args);

/**
 * command_help
 *
 * Displays a help message to the user, explaining the commands in for the
 * simulator and how to use them.
 **/
void command_help(cpu_state_t *cpu_state, char *args[], int num_args);

#endif /* COMMANDS_H_ */
