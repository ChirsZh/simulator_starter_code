/**
 * shell.c
 *
 * RISC-V 32-bit Instruction Level Simulator
 *
 * ECE 18-447
 * Carnegie Mellon University
 *
 * This file contains the shell for the simulator.
 *
 * The shell is the user-interactive interface that lets the user view the
 * processor state, run programs, and view other information about the program
 * being simulated on the processor.
 *
 * Authors:
 *  - 2016: Brandon Perez
 **/

/*----------------------------------------------------------------------------*
 *                          DO NOT MODIFY THIS FILE!                          *
 *          You should only add or change files in the src directory!         *
 *----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>          // Printf, fgets, and related functions
#include <stdint.h>         // Fixed-size integral types

#include <assert.h>         // Assert macro
#include <string.h>         // String manipulation functions and memset
#include <errno.h>          // Error codes and perror
#include <signal.h>         // Signal numbers and sigaction function

#include "sim.h"            // Interface to the core simulator, cpu_state_t
#include "memory_shell.h"   // Interface to the processor memory
#include "commands.h"       // Interface to the shell commands

/*----------------------------------------------------------------------------
 * Internal Definitions
 *----------------------------------------------------------------------------*/

// The expected number of command line arguments, including the program name
#define NUM_CMDLINE_ARGS    2

// The maximum line length the user can type in for a command
#define COMMAND_MAX_LEN     100

/* The maximum number of arguments that can be parsed from user input. This more
 * than the max possible, so too many arguments can be detected. */
#define COMMAND_MAX_ARGS    4

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
    fprintf(stdout, "Usage: riscv-sim <program>\n");
    fprintf(stdout, "Example: riscv-sim 447inputs/additest.s\n");
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
    if (argc != NUM_CMDLINE_ARGS) {
        fprintf(stderr, "Error: Improper number of command line arguments.\n");
        print_usage();
        return -EINVAL;
    }

    // Get the program path from the command line arguments
    *program_path = argv[1];
    return 0;
}

/*----------------------------------------------------------------------------
 * Signal Handling
 *----------------------------------------------------------------------------*/

/**
 * sigint_handler
 *
 * This function handles CTRL-C keystrokes from the user. Its purpose is to
 * prevent the program from being terminated when a CTRL-C is typed.
 **/
void sigint_handler(int signum)
{
    (void)signum;       // Silence the compiler
    return;
}

/**
 * setup_signals
 *
 * Setups up the signal handling for the simulator, which is simply handling
 * SIGINT's from CTRL-C keystrokes from the user.
 **/
static void setup_signals()
{
    // Setup the SIGINT handler
    struct sigaction sigact;
    memset(&sigact, 0, sizeof(sigact));
    sigact.sa_handler = sigint_handler;
    sigemptyset(&sigact.sa_mask);
    sigaction(SIGINT, &sigact, NULL);

    return;
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
    char *args[], int num_args, bool *quit)
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
    } else if (strcmp(command, "mem") == 0) {
        command_mem(cpu_state, args, num_args);
    } else if (strcmp(command, "rdump") == 0) {
        command_rdump(cpu_state, args, num_args);
    } else if (strcmp(command, "mdump") == 0) {
        command_mdump(cpu_state, args, num_args);
    } else if (strcmp(command, "restart") == 0) {
        command_restart(cpu_state, args, num_args);
    } else if (strcmp(command, "load") == 0) {
        command_load(cpu_state, args, num_args);
    } else if (strcmp(command, "quit") == 0) {
        *quit = command_quit(cpu_state, args, num_args);
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
        char *args[], int num_args, bool *quit)
{
    // Assume the command is not quit
    *quit = false;

    // The command must be exactly one character long for short commands
    if (strlen(command) != 1) {
        return false;
    }

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
            command_mem(cpu_state, args, num_args);
            return true;

        case 'q':
            *quit = command_quit(cpu_state, args, num_args);
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
        char *args[COMMAND_MAX_ARGS+1])
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
    char *args[COMMAND_MAX_ARGS+1];
    int num_args = split_command(command_string, &command, args);

    // The user entered an empty line, so there's no command to process
    if (command == NULL) {
        return false;
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
    // Initialize the buffer and buffer size value for getline
    char *line = NULL;
    size_t buf_size = 0;

    // Continuously process user commands until a quit or EOF
    while (true)
    {
        // Print the command prompt for the user
        fprintf(stdout, "RISC-V Sim> ");
        fflush(stdout);

        /* Read the next line from the user, only terminating on an EOF
         * character or an error that isn't EINTR. */
        int rc = getline(&line, &buf_size, stdin);
        if (rc < 0 && feof(stdin)) {
            fprintf(stdout, "\n");
            break;
        } else if (rc < 0 && errno != EINTR) {
            perror("\nError: Unable to read line of user input");
            break;
        } else if (rc < 0) {
            fprintf(stdout, "\n");
            clearerr(stdin);
            continue;
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
    memset(&cpu_state, 0, sizeof(cpu_state));
    rc = init_cpu_state(&cpu_state, program_path);
    if (rc < 0) {
        fprintf(stderr, "Failed to load the first program. Not starting the "
                "simulator.\n");
        return -rc;
    }

    // Setup the signal handling for the program
    setup_signals();

    // The REPL loop for the simulator, wait for and read user commands
    simulator_repl(&cpu_state);
    return 0;
}
