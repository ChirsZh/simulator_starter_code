/**
 * addtest.S
 *
 * Add Test
 *
 * This test exercises the various add instructions in the RISC-V ISA.
 * This also demonstrates how the preprocessor can be leveraged in *.S assembly
 * files.
 **/

/**
 * When files have a *.S extension, the compiler will run the preprocessor on
 * the file before assembling it. Thus, you can leverage the C preprocessor
 * (#define, #include, #ifdef, etc.) in your assembly code.
 *
 * Also, you can use C++-style comments (//) in addition to the normal assembly
 * comments (#, slash-*). Be careful with using #, because certain words
 * (e.g. if, define, etc.) may be interpreted as preprocessor directives.
 **/

// The initial value to seed the test with
#define TEST_SEED               512

    .text                       // Declare the code to be in the .text segment
    .global main                // Make main visible to the linker
main:
    addi  t0, zero, TEST_SEED   // t0 (x5) = TEST_SEED (512)
    add   t1, zero, t0          // t1 (x6) = t0
    add   t2, t1,   t1          // t2 (x7) = t1 + t1
    add   t3, t2,   t2          // t3 (x28) = t2 + t2

    addi  a0, zero, 0xa         // a0 (x10) = 0xa
    ecall                       // Terminate the simulation by passing 0xa to
                                // ecall in register a0 (x10).
