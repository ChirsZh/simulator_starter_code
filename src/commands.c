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
 *  - 2016: Brandon Perez
 **/

/*----------------------------------------------------------------------------*
 *                          DO NOT MODIFY THIS FILE!                          *
 *                 You should only add files or change sim.c!                 *
 *----------------------------------------------------------------------------*/

#include <stdlib.h>             // Malloc and related functions
#include <stdio.h>              // Printf and related functions
#include <stdint.h>             // Fixed-size integral types

#include <limits.h>             // Limits for integer types
#include <assert.h>             // Assert macro
#include <errno.h>              // Error codes and perror

#include "sim.h"                // Definition of cpu_state_t
#include "commands.h"           // This file's interface
#include "libc_extensions.h"    // Parsing functions, array_len, Snprintf

/*----------------------------------------------------------------------------
 * Shared Helper Functions
 *----------------------------------------------------------------------------*/

/**
 * print_separator
 *
 * Prints a line of line_width separators to the given file. This is used in the
 * dump commands to seperate headers from values.
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
 * open_dump_file
 *
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
 * close_dump_file
 *
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
 * Step and Go Commands
 *----------------------------------------------------------------------------*/

// The maximum number of arguments that can be specified to the step command
#define STEP_MAX_NUM_ARGS       1

// The expected number of arguments for the go command
#define GO_NUM_ARGS             0

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
void command_step(cpu_state_t *cpu_state, char *args[], int num_args)
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
void command_go(cpu_state_t *cpu_state, char *args[], int num_args)
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

// The minimum and maximum expected number of arguments for the reg command
#define REG_MIN_NUM_ARGS        1
#define REG_MAX_NUM_ARGS        2

// The maximum expected number of arguments for the rdump command
#define RDUMP_MAX_NUM_ARGS      1

// The maximum length of an ISA and ABI alias name for a register
#define ISA_NAME_MAX_LEN        3
#define ABI_NAME_MAX_LEN        5

// The maximum number of hexadecimal and decimal digits for a 32-bit integer
#define INT32_MAX_DIGITS        10
#define INT32_MAX_HEX_DIGITS    (2 * sizeof(uint32_t))

/* Define the format for a register dump line. This is the width of each column
 * in the, which is a max between a value and its column title. */
#define ISA_NAME_COL_LEN        max(ISA_NAME_MAX_LEN, string_len("ISA Name"))
#define ABI_NAME_COL_LEN        max(ABI_NAME_MAX_LEN + string_len("()"), \
                                        string_len("ABI NAME"))
#define REG_HEX_COL_LEN         max(INT32_MAX_HEX_DIGITS + string_len("0x"), \
                                        string_len("Hex Value"))
#define REG_UINT_COL_LEN        max(INT32_MAX_DIGITS + string_len("()"), \
                                        string_len("Uint Value"))
#define REG_INT_COL_LEN         max(INT32_MAX_DIGITS + string_len("()"), \
                                        string_len("Int Value"))

// Structure representing the naming information about a register
typedef struct register_name {
    const char *isa_name;       // The ISA name for a register (x0..x31)
    const char *abi_name;       // The ABI name for a register (sp, t0, etc.)
} register_name_t;

// An array of all the naming information for a register, and its ABI aliases
static const register_name_t RISCV_REGISTER_NAMES[RISCV_NUM_REGS] = {
    { .isa_name = "x0",  .abi_name = "zero", },
    { .isa_name = "x1",  .abi_name = "ra", },
    { .isa_name = "x2",  .abi_name = "sp", },
    { .isa_name = "x3",  .abi_name = "gp", },
    { .isa_name = "x4",  .abi_name = "tp", },
    { .isa_name = "x5",  .abi_name = "t0", },
    { .isa_name = "x6",  .abi_name = "t1", },
    { .isa_name = "x7",  .abi_name = "t2", },
    { .isa_name = "x8",  .abi_name = "s0/fp", },
    { .isa_name = "x9",  .abi_name = "s1", },
    { .isa_name = "x10", .abi_name = "a0", },
    { .isa_name = "x11", .abi_name = "a1", },
    { .isa_name = "x12", .abi_name = "a2", },
    { .isa_name = "x13", .abi_name = "a3", },
    { .isa_name = "x14", .abi_name = "a4", },
    { .isa_name = "x15", .abi_name = "a5", },
    { .isa_name = "x16", .abi_name = "a6", },
    { .isa_name = "x17", .abi_name = "a7", },
    { .isa_name = "x18", .abi_name = "s2", },
    { .isa_name = "x19", .abi_name = "s3", },
    { .isa_name = "x20", .abi_name = "s4", },
    { .isa_name = "x21", .abi_name = "s5", },
    { .isa_name = "x22", .abi_name = "s6", },
    { .isa_name = "x23", .abi_name = "s7", },
    { .isa_name = "x24", .abi_name = "s8", },
    { .isa_name = "x25", .abi_name = "s9", },
    { .isa_name = "x26", .abi_name = "s10", },
    { .isa_name = "x27", .abi_name = "s11", },
    { .isa_name = "x28", .abi_name = "t3", },
    { .isa_name = "x29", .abi_name = "t4", },
    { .isa_name = "x30", .abi_name = "t5", },
    { .isa_name = "x31", .abi_name = "t6", },
};

/**
 * find_register
 *
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
 * print_register_header
 *
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
    print_separator('-', line_width, file);
    return;
}

/**
 * print_register
 *
 * Prints out the information for a given register on one line to the file.
 **/
static void print_register(cpu_state_t *cpu_state, int reg_num, FILE *file)
{
    assert(0 <= reg_num && reg_num < (int)array_len(RISCV_REGISTER_NAMES));

    // Get the register value, and its register name struct
    const register_name_t *reg_name = &RISCV_REGISTER_NAMES[reg_num];
    uint32_t reg_value = cpu_state->regs[reg_num];

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
 * print_cpu_state
 *
 * Prints out information about the current CPU state to the file.
 **/
static void print_cpu_state(const cpu_state_t *cpu_state, FILE* file)
{
    ssize_t width = fprintf(file, "Current CPU State and Register Values:\n");
    print_separator('-', width-1, file);
    fprintf(file, "%-20s = %d\n", "Instruction Count", cpu_state->instr_count);
    fprintf(file, "%-20s = 0x%08x\n", "Program Counter (PC)", cpu_state->pc);
    return;
}

/**
 * command_reg
 *
 * Display the value of the specified register to the user. The user can
 * optionally specify a value to update the register's value instead.
 **/
void command_reg(cpu_state_t *cpu_state, char *args[], int num_args)
{
    assert(REG_MAX_NUM_ARGS - REG_MIN_NUM_ARGS == 1);
    assert(array_len(cpu_state->regs) == array_len(RISCV_REGISTER_NAMES));

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
    if (reg_num < 0 || reg_num >= (int)array_len(cpu_state->regs)) {
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
    cpu_state->regs[reg_num] = (uint32_t)reg_value;
    return;
}

/**
 * comand_rdump
 *
 * Displays the value of all registers in the system, along with the number of
 * instructions executed so far. The user can optionally specify a file to dump
 * the values to.
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

    // Print out the current CPU state, and the header for the registers
    print_cpu_state(cpu_state, dump_file);
    fprintf(dump_file, "\n");
    print_register_header(dump_file);

    // Print out all of the general purpose register values
    for (int i = 0; i < (int)array_len(cpu_state->regs); i++)
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
#define MEMORY_MIN_NUM_ARGS     1
#define MEMORY_MAX_NUM_ARGS     2

// The maximum expected number of arguments for the mdump command
#define MDUMP_MIN_NUM_ARGS      2
#define MDUMP_MAX_NUM_ARGS      3

static void print_memory_header(FILE* file)
{
    ssize_t line_width = fprintf(file, "%-10s  %-4s %-4s %-4s %-4s\n",
            "Address", "+3", "+2", "+1", "+0");
    print_separator('-', line_width-1, file);
    return;
}

/**
 * print_memory
 *
 * Prints out information for the given address on one line to the file. The
 * memory is printed out in little-endian order
 **/
static void print_memory(uint32_t address, uint8_t *memory, FILE* file)
{
    fprintf(file, "0x%08x: 0x%02x 0x%02x 0x%02x 0x%02x\n", address, memory[3],
            memory[2], memory[1], memory[0]);
    return;
}

/**
 * command_memory
 *
 * Displays the value of the specified memory address to the user. The user can
 * optionally specify a value to update the memory locations value instead.
 **/
void command_memory(cpu_state_t *cpu_state, char *args[], int num_args)
{
    if (num_args < MEMORY_MIN_NUM_ARGS) {
        fprintf(stderr, "Error: memory: Too few arguments specified.\n");
        return;
    } else if (num_args > MEMORY_MAX_NUM_ARGS) {
        fprintf(stderr, "Error: memory: Too many arguments specified.\n");
        return;
    }

    // First, try to parse the memory address
    const char *address_string = args[0];
    int32_t address;
    if (parse_int32(address_string, &address) < 0) {
        fprintf(stderr, "Error: memory: Unable to parse '%s' as a 32-bit "
                "integer.\n", address_string);
        return;
    }

    // Find and check that the address specified is valid.
    uint8_t *memory = mem_find_address(cpu_state, address);
    if (memory == NULL) {
        fprintf(stderr, "Error: memory: Invalid memory address 0x%08x "
                "specified.\n", address);
        return;
    }

    // If the user didn't specify a value, then we print the value
    if (num_args == MEMORY_MIN_NUM_ARGS) {
        print_memory_header(stdout);
        print_memory(address, memory, stdout);
        return;
    }

    // Otherwise, parse the second argument as a 32-bit integer
    const char *mem_value_string = args[1];
    int32_t mem_value;
    if (parse_int32(mem_value_string, &mem_value) < 0) {
        fprintf(stderr, "Error: memory: Unable to parse '%s' as a 32-bit "
                "integer.\n", mem_value_string);
        return;
    }

    // Update the memory location with the new value
    mem_write_word(memory, mem_value);
    return;
}

/**
 * command_mdump
 *
 * Displays the values of a range of memory locations in the system. The user
 * can optionally specify a file to dump the memory values to.
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
    if (end_addr < start_addr) {
        fprintf(stderr, "Error: mdump: Ending address is less than start "
                "address.\n");
        return;
    } else  if (!mem_range_valid(cpu_state, start_addr, end_addr)) {
        fprintf(stderr, "Error: mdump: Address range 0x%08x - 0x%08x is not "
                "valid.\n", start_addr, end_addr);
        return;
    }

    // Print out all the values in memory over the specified range
    print_memory_header(dump_file);
    for (uint32_t addr = start_addr; addr < end_addr; addr += sizeof(uint32_t))
    {
        uint8_t *mem_addr = mem_find_address(cpu_state, addr);
        assert(mem_addr != NULL);
        print_memory(addr, mem_addr, dump_file);
    }

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
#define LOAD_NUM_ARGS           1
#define RESTART_NUM_ARGS        0

/**
 * init_cpu_state
 *
 * Initializes the CPU state, and loads the specified program into the
 * processor's memory.
 **/
int init_cpu_state(cpu_state_t *cpu_state, char *program_path)
{
    // Clear out the CPU state, and initialize the CPU state fields
    cpu_state->instr_count = 0;
    memset(cpu_state->regs, 0, sizeof(cpu_state->regs));

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
 * command_restart
 *
 * Resets the processor and restarts the currently loaded program from its first
 * instruction.
 **/
void command_restart(cpu_state_t *cpu_state, char *args[], int num_args)
{
    // Silence unused variable warnings from the compiler
    (void)args;

    // Check that the appropiate nubmer of arguments was specified
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
 * command_load
 *
 * Resets the processor and loads a new program into the processor, replacing
 * the currently executing program. The execution starts from the beginning of
 * the loaded program.
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
 * Help and Quit Commands
 *----------------------------------------------------------------------------*/

// The expected number of arguments for the quit command
#define QUIT_NUM_ARGS           0

/**
 * command_quit
 *
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

/**
 * command_help
 *
 * Displays a help message to the user, explaining the commands in for the
 * simulator and how to use them.
 **/
void command_help(cpu_state_t *cpu_state, char *args[], int num_args)
{
    // Silence the compiler
    (void)cpu_state;
    (void)args;
    (void)num_args;

    return;
}
