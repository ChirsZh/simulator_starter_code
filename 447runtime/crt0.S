/**
 * crt0.S
 *
 * The startup file for assembly test programs. This handles setting up the
 * environment, calling main, and then telling the simulator the program is
 * done.
 *
 * Authors:
 *  - 2016: Brandon Perez
 **/

// The top 20 bits of the end address of the user's stack segment (exclusive)
#define USER_STACK_END          (0x7ff00000 >> (32 - 20))

/**
 * _start
 *
 * The entry point for all assembly programs. Handles setting up the
 * environment, invoking main, then ending the program
 **/
    .text
    .global _start
_start:
    lui sp, USER_STACK_END      // Initialize the stack pointer (x2)

    call main                   // Call the user's main program

    ecall                       // Terminate the program by doing a syscall.
                                // The return value a0 (x10) must be 0xa
