/**
 * crt0.s
 *
 * The startup file for assembly test programs. This handles setting up the
 * environment, calling main, and then telling the simulator the program is
 * done.
 *
 * Authors:
 *  - 2016: Brandon Perez
 **/

// The top 20 bits of the start of the user's stack segment address
#define USER_STACK_START    (0x7ff00000 >> (32 - 20))

/**
 * _start
 *
 * The entry point for all assembly programs. Handles setting up the
 * environment, invoking main, then ending the program
 **/
    .text
    .global _start
_start:
    lui sp, USER_STACK_START    // Initialize the stack pointer

    jal main                    // Call the user's main program

    addi x2, x0, 0xa            // Terminate the program
    ecall
