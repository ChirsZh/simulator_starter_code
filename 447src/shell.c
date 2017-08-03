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
 *  - 2016 - 2017: Brandon Perez
 **/

/*----------------------------------------------------------------------------*
 *                          DO NOT MODIFY THIS FILE!                          *
 *          You should only add or change files in the src directory!         *
 *----------------------------------------------------------------------------*/

// Standard Includes
#include <stdlib.h>             // C standard library
#include <stdio.h>              // Printf, fgets, and related functions
#include <stdint.h>             // Fixed-size integral types
#include <stdbool.h>            // Definition of the boolean type

// Standard Includes
#include <assert.h>             // Assert macro
#include <string.h>             // String manipulation functions and memset
#include <errno.h>              // Error codes and perror
#include <signal.h>             // Signal numbers and sigaction function

// Readline Includes
#include <readline/readline.h>  // Interface to the readline library
#include <readline/history.h>   // Interface to history for the readline library

// 18-447 Simulator Includes
#include <sim.h>                // Interface to the core simulator, cpu_state_t

// Local Includes
#include "commands.h"           // Interface to the shell commands
#include "memory_shell.h"       // Interface to the processor memory

/*----------------------------------------------------------------------------
 * Internal Definitions
 *----------------------------------------------------------------------------*/

// The expected number of command line arguments, including the program name
static const int NUM_CMDLINE_ARGS       = 2;

// The maximum line length the user can type in for a command
static const int COMMAND_MAX_LEN        = 100;

/* The maximum number of arguments that can be parsed from user input. This more
 * than the max possible, so too many arguments can be detected. */
static const int COMMAND_MAX_ARGS       = 4;

// The readline history file name, and the maximum number of lines for it
static const int HISTORY_MAX_LINES      = 100;
static const char *HISTORY_FILE         = ".riscv_sim_history";

// Indicates that a SIGINT signal was received by the program
volatile bool SIGINT_RECEIVED           = false;

/*----------------------------------------------------------------------------
 * Command Line Parsing
 *----------------------------------------------------------------------------*/

/**
 * Prints the usage message for the program.
 **/
static void print_usage()
{
    fprintf(stdout, "Usage: riscv-sim <program>\n");
    fprintf(stdout, "Example: riscv-sim 447inputs/additest.S\n");
    return;
}

/**
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
 * Signal Handling and Readline Setup
 *----------------------------------------------------------------------------*/

/**
 * This function handles CTRL-C keystrokes from the user. Its purpose is to
 * prevent the program from being terminated when a CTRL-C is typed, and to
 * indicate to any running `go` command to stop.
 **/
static void sigint_handler(int signum)
{
    (void)signum;       // Silence the compiler
    SIGINT_RECEIVED = true;
    return;
}

/**
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

/**
 * Sets up the readline library, reading the readline history file. This loads
 * the previously executed commands from other simulator invocations, so that
 * the user can access them inside the shell.
 **/
static int setup_readline(const char *history_file)
{
    int rc = read_history(history_file);
    if (rc != 0 && errno != ENOENT) {
        rc = -errno;
        fprintf(stderr, "Error: %s: Unable to open readline history file: "
                "%s.\n", history_file, strerror(errno));
    }
    return rc;
}

/**
 * Cleans up the readline library, appending the history list of command
 * executed during this session to the history file. The file is truncated to a
 * maximum number.
 **/
static int cleanup_readline(const char *history_file, int max_history_lines)
{
    // Open the history file in append mode, creating it if it doesn't exist
    FILE *history = fopen(history_file, "a");
    if (history == NULL) {
        int rc = -errno;
        fprintf(stderr, "Error: %s: Unable to open history file in append "
                "mode: %s.\n", history_file, strerror(errno));
        return rc;
    }
    fclose(history);

    // Append the most recent history elements to the history file
    int rc = append_history(max_history_lines, history_file);
    if (rc != 0) {
        rc = -errno;
        fprintf(stderr, "Error: %s: Unable to append history list to readline "
                "history file: %s.\n", history_file, strerror(errno));
        return rc;
    }

    // Truncate the history file to the max number of lines
    rc = history_truncate_file(history_file, max_history_lines);
    if (rc != 0) {
        rc = -errno;
        fprintf(stderr, "Error: %s: Unable to truncate readline history file: "
                "%s.\n", history_file, strerror(errno));
    }
    return rc;
}

/*----------------------------------------------------------------------------
 * Simulator REPL
 *----------------------------------------------------------------------------*/

/**
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

    // Run the appropriate command based on what the user specified
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

    // Based on the first character, run the appropriate command
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
 * Splits the user input command string into a command and argument list. The
 * command returned is a typical null-terminated string, while the argument list
 * is terminated by a NULL pointer. The length of the argument list is returned
 * returned.
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
 * Attempts to parse and process the command specified by the user as either the
 * long form of the command, a string), or the short form, a single character.
 * Returns true if the quit command was specified.
 **/
static bool process_command(cpu_state_t *cpu_state, char *command_string)
{
    // Separate the command string into the command and a list of arguments
    char *command;
    char *args[COMMAND_MAX_ARGS+1];
    int num_args = split_command(command_string, &command, args);

    // The user entered an empty line, so there's no command to process
    if (command == NULL) {
        return false;
    }

    /* Otherwise, identify the command based on its short alias or long form.
     * If the command is valid, then add it to readline's history. */
    bool quit;
    if (process_long_command(cpu_state, command, args, num_args, &quit)) {
        return quit;
    } else if (process_short_command(cpu_state, command, args, num_args,
                &quit)) {
        return quit;
    } else {
        fprintf(stderr, "Error: Invalid command '%s' specified.\n", command);
        fprintf(stdout, "To see a complete listing of commands, type '?' or "
                "'help'.\n");
        return false;
    }
}

/**
 * The read-eval-print loop (REPL) for the simulator. Continuously waits for
 * user input, and performs the specified command, until the user specifies quit
 * or sends an EOF character.
 **/
static void simulator_repl(cpu_state_t *cpu_state)
{
    // Continuously process user commands until a quit or EOF
    while (true)
    {
        /* Read the next line from the user, terminating on an EOF, and add it
         * to readline's history if it's not an EOF. */
        char *line = readline("RISC-V Sim> ");
        if (line == NULL) {
            fprintf(stdout, "\n");
            break;
        }
        add_history(line);

        // Process the user's command, stop if the user requested quit
        bool quit = process_command(cpu_state, line);
        free(line);
        if (quit) {
            break;
        }
    }

    return;
}

/**
 * The main method for the simulator.
 *
 * This parses the command line arguments, initializes the CPU and starts up the
 * REPL for the simulator.
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

    // Setup the readline library
    rc = setup_readline(HISTORY_FILE);
    if (rc < 0) {
        return -rc;
    }

    // The REPL loop for the simulator, wait for and read user commands
    simulator_repl(&cpu_state);

    // Cleanup the readline library
    return -cleanup_readline(HISTORY_FILE, HISTORY_MAX_LINES);
}
