/**
 * commands.c
 *
 * RISC-V 32-bit Instruction Level Simulator
 *
 * ECE 18-447
 * Carnegie Mellon University
 *
 * This file contains the implementation for the shell commands.
 *
 * The commands are how the user interacts with the simulator from the shell.
 * The commands are pretty basic, such as stepping the program, displaying
 * registers, etc.
 *
 * Authors:
 *  - 2016 - 2017: Brandon Perez
 **/

/*----------------------------------------------------------------------------*
 *                          DO NOT MODIFY THIS FILE!                          *
 *          You should only add or change files in the src directory!         *
 *----------------------------------------------------------------------------*/

// Standard Includes
#include <stdlib.h>                 // Malloc and related functions
#include <stdio.h>                  // Printf and related functions
#include <stdbool.h>                // Definition of the boolean type
#include <stdint.h>                 // Fixed-size integral types

// Standard Includes
#include <limits.h>                 // Limits for integer types
#include <assert.h>                 // Assert macro
#include <errno.h>                  // Error codes and perror

// 18-447 Simulator Includes
#include <sim.h>                    // Definition of cpu_state_t
#include <register_file.h>          // Interface to the register file

// Local Includes
#include "memory_shell.h"           // Interface to the processor memory
#include "libc_extensions.h"        // Parsing functions, array_len, Snprintf
#include "riscv_register_names.h"   // Names for the RISC-V registers
#include "commands.h"               // This file's interface

/*----------------------------------------------------------------------------
 * Shared Helper Functions
 *----------------------------------------------------------------------------*/

/**
 * Prints a line of line_width separators to the given file. This is used in the
 * dump commands to separate headers from values.
 **/
static void print_separator(char separator, ssize_t line_width, FILE* file)
{
    char separator_line[line_width+1];
    memset(separator_line, separator, sizeof(separator_line));
    separator_line[line_width] = '\0';
    fprintf(file, "%s\n", separator_line);
    return;
}

/**
 * Opens the dump file if it was specified by the user, otherwise defaults to
 * stdout. Returns NULL on error.
 **/
static FILE *open_dump_file(char *args[], int num_args,
        int dumpfile_arg_num, const char *cmd)
{
    // Default to stdout if there aren't enough arguments
    if (!(dumpfile_arg_num < num_args)) {
        return stdout;
    }

    // Otherwise, try to open the specified dump file
    FILE *dump_file = fopen(args[dumpfile_arg_num], "w");
    if (dump_file == NULL) {
        fprintf(stderr, "Error: %s: %s: Unable to open file: %s.\n", cmd,
                args[dumpfile_arg_num], strerror(errno));
    }
    return dump_file;
}

/**
 * Closes a previously opened dump file. If the dump file is stdout, then
 * nothing is done.
 **/
static void close_dump_file(FILE *dump_file)
{
    if (dump_file != stdout) {
        fclose(dump_file);
    }
    return;
}

/*----------------------------------------------------------------------------
 * Verbose, Step, and Go Commands
 *----------------------------------------------------------------------------*/

// The maximum number of arguments that can be specified to the step command
static const int STEP_MAX_NUM_ARGS      = 1;

// The expected number of arguments for the go command
static const int GO_NUM_ARGS            = 0;

/**
 * Run the simulator for a single cycle, incrementing the instruction count.
 **/
static void run_simulator(cpu_state_t *cpu_state)
{
    // Run the simulator for a cycle, then the increment instruction count
    process_instruction(cpu_state);
    cpu_state->cycle += 1;

    // If the user has activated verbose mode, then perform a register dump
    if (cpu_state->verbose_mode) {
        command_rdump(cpu_state, NULL, 0);
    }
    return;
}

/**
 * Runs the simulator for a specified number of cycles or until a halt.
 *
 * The user can optionally specify the number of cycles. Otherwise, the default
 * is to run the processor one cycle. If the processor is halted before the
 * number of steps is reached, then simulation stops.
 **/
void command_step(cpu_state_t *cpu_state, char *args[], int num_args)
{
    // Check that the appropriate number of arguments was specified
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
 * Runs the simulator until program completion or an exception is encountered.
 *
 * In the case of an infinite running program because of a bug in the
 * implementation, the user can interrupt execution with a keyboard interrupt.
 **/
void command_go(cpu_state_t *cpu_state, char *args[], int num_args)
{
    // Silence unused variable warnings from the compiler
    (void)args;

    // Check that the appropriate number of arguments was specified
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

    /* Run the simulator until the processor is halted or the user tells us to
     * stop with a keyboard interrupt (SIGINT). */
    SIGINT_RECEIVED = false;
    while (!cpu_state->halted && !SIGINT_RECEIVED)
    {
        run_simulator(cpu_state);
    }

    // Tell the user if they interrupted execution, and reset the received flag
    if (SIGINT_RECEIVED) {
        fprintf(stdout, "\nExecution interrupted by the user, stopping.\n");
    }
    SIGINT_RECEIVED = false;

    return;
}

/*----------------------------------------------------------------------------
 * Reg and Rdump Commands
 *----------------------------------------------------------------------------*/

// The minimum and maximum expected number of arguments for the reg command
static const int REG_MIN_NUM_ARGS       = 1;
static const int REG_MAX_NUM_ARGS       = 2;

// The maximum expected number of arguments for the rdump command
static const int RDUMP_MAX_NUM_ARGS     = 1;

// The maximum length of an ISA and ABI alias name for a register
#define ISA_NAME_MAX_LEN                3
#define ABI_NAME_MAX_LEN                5

// The maximum number of hexadecimal and decimal digits for a 32-bit integer
#define INT32_MAX_DIGITS                10
#define INT32_MAX_HEX_DIGITS            (2 * sizeof(uint32_t))

/* Define the format for a register dump line. This is the width of each column
 * in the, which is a max between a value and its column title. */
static const size_t ISA_NAME_COL_LEN    = max(ISA_NAME_MAX_LEN,
        string_len("ISA Name"));
static const size_t ABI_NAME_COL_LEN    = max(ABI_NAME_MAX_LEN +
        string_len("()"), string_len("ABI Name"));
static const size_t REG_HEX_COL_LEN     = max(INT32_MAX_HEX_DIGITS +
        string_len("0x"), string_len("Hex Value"));
static const size_t REG_UINT_COL_LEN    = max(INT32_MAX_DIGITS +
        string_len("()"), string_len("Uint Value"));
static const size_t REG_INT_COL_LEN     = max(INT32_MAX_DIGITS +
        string_len("()") + 1, string_len("Int Value"));

/**
 * Tries to find the register with a matching ISA name or ABI alias from the
 * available registers. Returns a register number [0..31] on success, or a
 * negative number on failure.
 **/
static int find_register(const char *reg_name)
{
    // Iterate over each register, and try to find a match to the name
    for (int i = 0; i < (int)array_len(RISCV_REGISTER_NAMES); i++)
    {
        const register_name_t *reg_info = &RISCV_REGISTER_NAMES[i];
        if (strcmp(reg_name, reg_info->isa_name) == 0) {
            return i;
        } else if (strcmp(reg_name, reg_info->abi_name) == 0) {
            return i;
        }
    }

    // No matching registers were found
    return -ENOENT;
}

/**
 * Prints out the header lines for a register dump to the given file. This
 * contains the titles for each column of a line of register output.
 **/
static void print_register_header(FILE* file)
{
    ssize_t line_width = fprintf(file, "%-*s %-*s   %-*s %-*s %-*s\n",
            (int)ISA_NAME_COL_LEN, "ISA Name", (int)ABI_NAME_COL_LEN,
            "ABI Name", (int)REG_HEX_COL_LEN, "Hex Value",
            (int)REG_UINT_COL_LEN, "Uint Value", (int)REG_INT_COL_LEN,
            "Int Value");
    print_separator('-', line_width-1, file);
    return;
}

/**
 * Prints out the information for a given register on one line to the file.
 **/
static void print_register(cpu_state_t *cpu_state, riscv_reg_t reg_num,
        FILE *file)
{
    assert(0 <= reg_num && reg_num < (int)array_len(RISCV_REGISTER_NAMES));

    // Get the register value, and its register name struct
    const register_name_t *reg_name = &RISCV_REGISTER_NAMES[reg_num];
    uint32_t reg_value = register_read(cpu_state, reg_num);

    // Format the ABI alias name for the register surrounded with parenthesis
    char abi_name[ABI_NAME_COL_LEN+1];
    Snprintf(abi_name, sizeof(abi_name), "(%s)", reg_name->abi_name);

    // Format the hexadecimal, signed, and unsigned views of the register
    char reg_hex_value[REG_HEX_COL_LEN+1];
    char reg_uint_value[REG_UINT_COL_LEN+1];
    char reg_int_value[REG_INT_COL_LEN+1];
    Snprintf(reg_hex_value, sizeof(reg_hex_value), "0x%08x", reg_value);
    Snprintf(reg_uint_value, sizeof(reg_uint_value), "(%u)", reg_value);
    Snprintf(reg_int_value, sizeof(reg_int_value), "(%d)", (int32_t)reg_value);

    // Print out the register names and its values
    fprintf(file, "%-*s %-*s = %-*s %-*s %-*s\n", (int)ISA_NAME_COL_LEN,
            reg_name->isa_name, (int)ABI_NAME_COL_LEN, abi_name,
            (int)REG_HEX_COL_LEN, reg_hex_value, (int)REG_UINT_COL_LEN,
            reg_uint_value, (int)REG_INT_COL_LEN, reg_int_value);
    return;
}

/**
 * Prints out information about the current CPU state to the file.
 **/
static void print_cpu_state(const cpu_state_t *cpu_state, FILE* file)
{
    ssize_t width = fprintf(file, "Current CPU State and Register Values:\n");
    print_separator('-', width-1, file);
    fprintf(file, "%-20s = %d\n", "Cycle", cpu_state->cycle);
    fprintf(file, "%-20s = 0x%08x\n", "Program Counter (PC)", cpu_state->pc);
    return;
}

/**
 * Display the value of the specified register to the user.
 *
 * The user can optionally specify a value, in which case, the command
 * updates the register's value instead of reading it.
 **/
void command_reg(cpu_state_t *cpu_state, char *args[], int num_args)
{
    assert(REG_MAX_NUM_ARGS - REG_MIN_NUM_ARGS == 1);
    assert(array_len(cpu_state->registers) == array_len(RISCV_REGISTER_NAMES));

    // Check that the appropriate number of arguments was specified
    if (num_args < REG_MIN_NUM_ARGS) {
        fprintf(stderr, "Error: reg: Too few arguments specified.\n");
        return;
    } else if (num_args > REG_MAX_NUM_ARGS) {
        fprintf(stderr, "Error: reg: Too many arguments specified.\n");
        return;
    }

    /* First, try to parse the register argument as an integer, then try to
     * parse it as string for one of its names. */
    const char *reg_string = args[0];
    int reg_num = -ENOENT;
    if (parse_int(reg_string, &reg_num) < 0) {
        reg_num = find_register(reg_string);
    }

    // If we couldn't parse the given register, or it is out of range, stop
    if (reg_num < 0 || reg_num >= (int)array_len(cpu_state->registers)) {
        fprintf(stderr, "Error: reg: Invalid register '%s' specified.\n",
                reg_string);
        return;
    }

    // If the user didn't specify a value, then we simply print the register out
    if (num_args == REG_MIN_NUM_ARGS) {
        print_register_header(stdout);
        print_register(cpu_state, reg_num, stdout);
        return;
    }

    // Otherwise, parse the second argument as a 32-bit integer
    const char *reg_value_string = args[1];
    int32_t reg_value;
    if (parse_int32(reg_value_string, &reg_value) < 0) {
        fprintf(stderr, "Error: reg: Unable to parse '%s' as a 32-bit "
                "integer.\n", reg_value_string);
        return;
    }

    // Update the register with the new value
    register_write(cpu_state, (riscv_reg_t)reg_num, reg_value);
    return;
}

/**
 * Displays the value of all the CPU registers, along with other information.
 *
 * The PC value and number of instructions executed so far are also displayed
 * with the register values. The user can optionally specify a file to which to
 * dump the register values.
 **/
void command_rdump(cpu_state_t *cpu_state, char *args[], int num_args)
{
    // Check that the appropriate number of arguments was specified
    if (num_args > RDUMP_MAX_NUM_ARGS) {
        fprintf(stderr, "Error: rdump: Too many arguments specified.\n");
        return;
    }

    // Open the dump file, defaulting to stdout if it is not specified
    int arg_num = RDUMP_MAX_NUM_ARGS - 1;
    FILE *dump_file = open_dump_file(args, num_args, arg_num, "rdump");
    if (dump_file == NULL) {
        return;
    }

    /* Print out the current CPU state, and the header for the registers.
     * The current CPU state is not printed to dump files. */
    if (dump_file == stdout) {
        print_cpu_state(cpu_state, dump_file);
        fprintf(dump_file, "\n");
    }
    print_register_header(dump_file);

    // Print out all of the general purpose register values
    for (int i = 0; i < (int)array_len(cpu_state->registers); i++)
    {
        print_register(cpu_state, i, dump_file);
    }

    // Close the dump file if it was specified by the user
    close_dump_file(dump_file);
    return;
}

/*----------------------------------------------------------------------------
 * Memory and Mdump Commands
 *----------------------------------------------------------------------------*/

// The minimum and maximum expected number of arguments for the memory command
static const int MEMORY_MIN_NUM_ARGS    = 1;
static const int MEMORY_MAX_NUM_ARGS    = 2;

// The maximum expected number of arguments for the mdump command
static const int MDUMP_MIN_NUM_ARGS     = 2;
static const int MDUMP_MAX_NUM_ARGS     = 3;

/**
 * Prints out the header lines for a memory dump to the given file. This
 * contains the titles for each column of a line of the memory dump.
 **/
static void print_memory_header(const mem_segment_t *segment, FILE *file)
{
    fprintf(file, "Segment: %s\n", segment->name);
    ssize_t line_width = fprintf(file, "%-10s  %-4s %-4s %-4s %-4s\n",
            "Address", "+0", "+1", "+2", "+3");
    print_separator('-', line_width-1, file);
    return;
}

/**
 * Prints out information for the given address on one line to the file. The
 * memory is printed out in little-endian order. Exactly print_bytes bytes of
 * the memory are displayed. If needed, the input address is rounded down to the
 * nearest 4-byte boundary.
 **/
static void print_memory_range(const mem_segment_t *segment,
        uint32_t start_addr, uint32_t end_addr, FILE *file)
{
    // Print out the header for memory
    print_memory_header(segment, file);

    // Determine the aligned starting address, and get a handle the memory
    uint32_t aligned_start = start_addr / sizeof(uint32_t) * sizeof(uint32_t);
    const uint8_t *mem_addr = &segment->mem[start_addr - segment->base_addr];

    // Print out the data between the starting and ending addresses
    for (uint32_t addr = aligned_start; addr < end_addr; addr++)
    {
        // When the address hits a 4-byte boundary, print out the address
        if (addr % sizeof(uint32_t) == 0 && addr != aligned_start) {
            fprintf(file, "\n0x%08x: ", addr);
        } else if (addr % sizeof(uint32_t) == 0) {
            fprintf(file, "0x%08x: ", addr);
        }

        // Print out the memory value if we're in the memory range
        if (addr >= start_addr) {
            fprintf(file, "0x%02x ", mem_addr[addr - start_addr]);
        } else {
            fprintf(file, "%4s ", "");
        }
    }
    fprintf(file, "\n");

    return;
}

/**
 * Displays the value of the specified memory address to the user.
 *
 * The user can optionally specify a value, in which case, the command updates
 * the memory location's value instead of reading it.
 **/
void command_mem(cpu_state_t *cpu_state, char *args[], int num_args)
{
    if (num_args < MEMORY_MIN_NUM_ARGS) {
        fprintf(stderr, "Error: mem: Too few arguments specified.\n");
        return;
    } else if (num_args > MEMORY_MAX_NUM_ARGS) {
        fprintf(stderr, "Error: mem: Too many arguments specified.\n");
        return;
    }

    // First, try to parse the memory address
    const char *address_string = args[0];
    int32_t addr;
    if (parse_int32(address_string, &addr) < 0) {
        fprintf(stderr, "Error: mem: Unable to parse '%s' as a 32-bit "
                "integer.\n", address_string);
        return;
    }

    // Find and check that the address specified is valid.
    mem_segment_t *segment = mem_find_segment(cpu_state, addr);
    if (segment == NULL) {
        fprintf(stderr, "Error: mem: Invalid memory address 0x%08x "
                "specified.\n", addr);
        return;
    }

    // If the user didn't specify a value, then we print the value
    uint32_t end_addr = min(addr + sizeof(uint32_t),
            segment->base_addr + segment->size);
    if (num_args == MEMORY_MIN_NUM_ARGS) {
        print_memory_range(segment, addr, end_addr, stdout);
        return;
    }

    // Otherwise, parse the second argument as a 32-bit integer
    const char *mem_value_string = args[1];
    int32_t mem_value;
    if (parse_int32(mem_value_string, &mem_value) < 0) {
        fprintf(stderr, "Error: mem: Unable to parse '%s' as a 32-bit "
                "integer.\n", mem_value_string);
        return;
    }

    // Update the memory location with the new value
    mem_write_word(segment, addr, mem_value);
    return;
}

/**
 * Displays the values of a range of memory locations in the system.
 *
 * The user can optionally specify a file to which to dump the memory values.
 **/
void command_mdump(cpu_state_t *cpu_state, char *args[], int num_args)
{
    // Check that the appropriate number of arguments was specified
    if (num_args < MDUMP_MIN_NUM_ARGS) {
        fprintf(stderr, "Error: mdump: Too few arguments specified.\n");
        return;
    } else if (num_args > MDUMP_MAX_NUM_ARGS) {
        fprintf(stderr, "Error: mdump: Too many arguments specified.\n");
    }

    // Parse the starting and ending addresses for the memory dump
    uint32_t start_addr, end_addr;
    const char *start_addr_string = args[0];
    if (parse_uint32_hex(start_addr_string, &start_addr) < 0) {
        fprintf(stderr, "Error: mdump: Unable to parse '%s' as a 32-bit "
                "unsigned hexadecimal integer.\n", start_addr_string);
        return;
    }
    const char *end_addr_string = args[1];
    if (parse_uint32_hex(end_addr_string, &end_addr) < 0) {
        fprintf(stderr, "Error: mdump: Unable to parse '%s' as a 32-bit "
                "unsigned hexadecimal integer.\n", end_addr_string);
        return;
    }

    // Open the dump file, defaulting to stdout if it is not specified
    int arg_num = MDUMP_MAX_NUM_ARGS - 1;
    FILE *dump_file = open_dump_file(args, num_args, arg_num, "mdump");
    if (dump_file == NULL) {
        return;
    }

    // Check that the start address is less than the end, and the range is valid
    if (!(start_addr < end_addr)) {
        fprintf(stderr, "Error: mdump: End address is not larger than the "
                "start address.\n");
        return;
    } else if (!mem_range_valid(cpu_state, start_addr, end_addr)) {
        fprintf(stderr, "Error: mdump: Address range 0x%08x - 0x%08x is not "
                "valid.\n", start_addr, end_addr);
        return;
    }

    // Find the memory segment to which the range belongs, and print it out
    const mem_segment_t *segment = mem_find_segment(cpu_state, start_addr);
    assert(segment != NULL);
    print_memory_range(segment, start_addr, end_addr, dump_file);

    // Close the dump file if was specified by the user (not stdout)
    if (dump_file != stdout) {
        fclose(dump_file);
    }
    return;
}

/*----------------------------------------------------------------------------
 * Restart and Load Commands
 *----------------------------------------------------------------------------*/

// The expected number of arguments for the load and restart commands
static const int LOAD_NUM_ARGS          = 1;
static const int RESTART_NUM_ARGS       = 0;

/**
 * Initializes the CPU state.
 *
 * This loads the specified program into the processor's memory, clearing out
 * any prior loaded program. Additionally, the register values are reset to
 * their proper starting values.
 **/
int init_cpu_state(cpu_state_t *cpu_state, char *program_path)
{
    assert(cpu_state->memory.segments != NULL);

    // Clear out the CPU state, and initialize the CPU state fields
    cpu_state->cycle = 0;
    memset(cpu_state->registers, 0, sizeof(cpu_state->registers));

    // Strip the extension from the program path, if there is one
    char *extension_start = strrchr(program_path, '.');
    if (extension_start != NULL) {
        extension_start[0] = '\0';
    }

    // Initialize the memory subsystem, and load the program into memory
    int rc = mem_load_program(cpu_state, program_path);
    if (rc < 0) {
        cpu_state->halted = true;
        return rc;
    }

    // Mark the CPU as running, and save the name of the loaded program
    cpu_state->halted = false;
    cpu_state->program = program_path;

    return rc;
}

/**
 * Resets the processor and restarts the currently loaded program.
 *
 * The program is naturally started again from its first instruction, with all
 * register and memory values reset to their starting values.
 **/
void command_restart(cpu_state_t *cpu_state, char *args[], int num_args)
{
    // Silence unused variable warnings from the compiler
    (void)args;

    // Check that the appropriate number of arguments was specified
    if (num_args != RESTART_NUM_ARGS) {
        fprintf(stderr, "Error: restart: Improper number of arguments "
                "specified.\n");
        return;
    }

    // Unload the program on the processor
    mem_unload_program(cpu_state);

    // Reinitialize the CPU state and reload the program, exit on failure
    int rc = init_cpu_state(cpu_state, cpu_state->program);
    if (rc < 0) {
        fprintf(stderr, "Error: restart: Unable to restart program. Exiting "
                "the simulator.\n");
        exit(rc);
    }

    return;
}

/**
 * Loads a new program into the processor's memory to be executed.
 *
 * This resets the processor and loads a new program into the processor,
 * replacing the currently executing program. The execution starts from the
 * beginning of the loaded program, with all memory and register values at their
 * proper starting values.
 **/
void command_load(cpu_state_t *cpu_state, char *args[], int num_args)
{
    // Check that the appropriate number of arguments was specified
    if (num_args != LOAD_NUM_ARGS) {
        fprintf(stderr, "Error: load: Improper number of arguments "
                "specified.\n");
        return;
    }

    // Unload the current program on the processor
    mem_unload_program(cpu_state);

    // Re-initialize the CPU state, and load the new program
    // Re-initialize the CPU state
    char *new_program = args[0];
    int rc = init_cpu_state(cpu_state, new_program);
    if (rc < 0) {
        fprintf(stderr, "Error: load: Unable to load program. Halting the "
                "simulator.\n");
    }

    return;
}

/*----------------------------------------------------------------------------
 * Verbose and Quit Commands
 *----------------------------------------------------------------------------*/

// The expected number of arguments for the verbose command
static const int VERBOSE_NUM_ARGS       = 0;

// The expected number of arguments for the quit command
static const int QUIT_NUM_ARGS          = 0;

/**
 * Toggles verbose mode for the simulator.
 *
 * If verbose mode is active, then the simulator performs a register dump after
 * every cycle that the CPU runs. This can be useful to do a cycle-by-cycle
 * diff between a reference implementation and this implementation.
 **/
void command_verbose(cpu_state_t *cpu_state, char *args[], int num_args)
{
    // Silence unused variable warnings from the compiler
    (void)args;

    // Check that the appropriate number of arguments was specified
    if (num_args != VERBOSE_NUM_ARGS) {
        fprintf(stderr, "Error: Improper number of arguments specified to "
                "'verbose' command.\n");
        return;
    }

    // Toggle the verbose mode for the processor
    cpu_state->verbose_mode = !cpu_state->verbose_mode;
    return;
}

/**
 * Quits the simulator.
 **/
bool command_quit(cpu_state_t *cpu_state, char *args[], int num_args)
{
    // Silence unused variable warnings from the compiler
    (void)args;

    if (num_args != QUIT_NUM_ARGS) {
        fprintf(stderr, "Error: quit: Improper number of arguments "
                "specified.\n");
        return false;
    }

    // Indicate that we should quit
    cpu_state->halted = true;
    return true;
}

/*----------------------------------------------------------------------------
 * Help Command
 *----------------------------------------------------------------------------*/

// The expected number of arguments for the help command
static const int HELP_NUM_ARGS          = 0;

/**
 * Prints out the header lines for the help message for the RISC-V simulator.
 **/
static void print_help_header()
{
    ssize_t line_width = fprintf(stdout, "\nRISC-V Simulator Help:\n");
    print_separator('-', line_width-2, stdout);
    return;
}

/**
 * Prints out the help message for the specified command, giving its usage and
 * the description for how the command is used.
 **/
static void print_help(const char *cmd_usage, const char *help_message)
{
    fprintf(stdout, "%-37s - %s\n", cmd_usage, help_message);
}

/**
 * Displays a help message to the user.
 *
 * The message explains the commands available in the simulator and how to use
 * them.
 **/
void command_help(cpu_state_t *cpu_state, char *args[], int num_args)
{
    // Silence unused variable warnings from the compiler
    (void)cpu_state;
    (void)args;

    // Check that the proper number of command line arguments were specified
    if (num_args != HELP_NUM_ARGS) {
        fprintf(stderr, "Error: help: Improper number of arguments "
                "specified.\n");
        return;
    }

    // Print the header and help message for the simulator commands
    print_help_header();
    print_help("s[tep] [cycles]", "Run the processor for one or the specified "
            "number of cycles, or until it is halted.");
    print_help("go", "Run the simulator until the processor is halted.");

    // Print help messages for register commands
    print_help("r[eg] <isa_name|abi_name|num> [value]", "Display the "
            "register's value or update it with a value.");
    print_help("rdump [dump_file]", "Display the CPU state and all registers, "
            "optionally dumping it to the file.");

    // Print help messages for memory commands
    print_help("m[em] <addr> [value]", "Display the memory address's value or "
            "update it with a value.");
    print_help("mdump <start> <end> [dump_file]", "Display the memory values "
            "across the range [start, end), optionally dumping it to the "
            "file.");

    // Print help messages for the load and restart commands
    print_help("restart", "Reset the processor and restart the program from "
            "the beginning.");
    print_help("load <program>", "Reset the processor and load the new program "
            "into memory for execution.");

    // Print help message for the verbose, quit, and help commands
    print_help("v[erbose]", "Toggles verbose mode for the simulator. When "
            "active, the simulator dumps the registers after each cycle.");
    print_help("q[uit]", "Quit the simulator. Can also be done with an EOF "
            "(CTRL-D).");
    print_help("h[elp]|?", "Display this help message.");
    fprintf(stdout, "\n");

    return;
}
