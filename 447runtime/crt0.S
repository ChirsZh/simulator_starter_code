/**
 * crt0.S
 *
 * The startup file for C test programs. This handles interacting with the
 * simulator environment. This calls main, then invokes `ecall` to tell the
 * simulator that the program is done.
 *
 * The return value from main is used as the argument to `ecall`. This must
 * match the halt argument value (0xa).
 *
 * Note that the simulator sets up the sp (x2) register, so the startup code
 * does not need to do this.
 *
 * Authors:
 *  - 2016: Brandon Perez
 **/

/**
 * _start
 *
 * The entry point for all assembly programs. Handles invoking main, then
 * ending the program
 **/
    .text           // Declare the code to be in the .text segment
    .global _start  // Make _start visible to the linker
_start:
    call main       // Call the user's main program. `call` is `jal x1, offset`

    ecall           // Terminate the program by doing a syscall. The return
                    // value a0 (x10) must be 0xa
