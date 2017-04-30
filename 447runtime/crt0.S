/**
 * crt0.S
 *
 * The startup file for C test programs. This handles interacting with the
 * simulator environment. This calls main, then invokes `ecall` to tell the
 * simulator that the program is done.
 *
 * The return value from main is placed into register t0 (x5). Regardless of
 * the return value, the starter code ensures that the simulator terminates
 * simulation by passing the appropiate value to ecall.
 *
 * Note that the simulator sets up the sp (x2) and gp (x3) registers, so the
 * startup code does not need to do this.
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
    .text                   // Declare the code to be in the .text segment
    .global _start          // Make _start visible to the linker
_start:
    call    main            // Call the user's program
    addi    x2, a0, 0       // Put the return value's lower 32-bits in x2 (sp)
    addi    x3, a1, 0       // Put the return value's upper 32-bits in x3 (gp)

    addi    a0, zero, 0xa   // a0 (x10) = 0xa
    ecall                   // Terminate the program by doing a syscall. The
                            // return value a0 (x10) must be 0xa
