# addtest.S
#
# Add Test
#
# This test exercises the various add instructions in the RISC-V ISA.
# This also demonstrates how *.S files differ from *.s files.

/**
 * When files have a *.S (vs *.s) extension, the compiler will run the
 * preprocessor on the file before assembling it. Thus, you can leverage the
 * C preprocessor (#define, #include, #ifdef, etc.) in your assembly code.
 *
 * Also, you can use C-style comments (// and /*) in addition to the normal
 * assembly comments (#).
 **/

// The initial value to seed the test with
#define TEST_SEED				512

    .text                       // Declare the code to be in the .text segment
    .global main                // Make main visible to the linker
main:
    addi t0, zero, TEST_SEED    // t0 (x5) = TEST_SEED (512)
    add t1, t0, zero			// t1 (x6) = t0
    add t2, t1, t1				// t2 (x7) = t1 + t1
    add t3, t2, t2				// t3 (x28) = t2 + t2

    addi a0, zero, 0xa          // a0 (x10) = 0xa
    ret                         // Return 0xa to the startup code to terminate
                                // the simulator. `ret` is `jalr x0, x1, 0`.
