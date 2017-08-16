/**
 * memtest0.S
 *
 * Basic Memory Test
 *
 * This performs a very basic test of the load and store instructions, checking
 * if word-wise loads and stores work. This test also demonstrates how to
 * declare global variables in assembly that go into the .data section and how
 * to interact with it.
 **/

/**
 * The simulator will allocate as many bytes as are declared for .data section,
 * so you must explicitly specify the size of the .data section as done below.
 * At program start, the simulator will set the value of the global pointer to
 * the gp (x1) register, which is the start of the .data section.
 *
 * The .space directive will allocate uninitialized memory for a memory segment.
 * The .word, .halfword, .bytes, .asciiz will allocate memory for specific
 * values. See the GNU Assembler (GAS) guide for a complete description.
 **/

// The size of the data section accessed, in bytes
#define DATA_SEG_SIZE           20

    .data                       // Declare items to be in the .data segment
data:                           // Symbol representing the start of .data
    .space  DATA_SEG_SIZE       // Allocate 20 bytes for the .data segment
data_end:                       // Symbol representing the end of .data

    .text                       // Declare the code to be in the .text segment
    .global main                // Make main visible to the linker
main:
    add     x1,  zero,  gp      // At program start gp has the start of the
                                // .data segment. x1 = gp (x3)

    addi    x5,  zero,  255     // x5 = 255
    add     x6,  x5,    x5      // x6 = x5 + x5
    add     x7,  x6,    x6      // x7 = x6 + x6
    addi    x8,  x7,    2000    // x8 = x7 + 2000

    sw      x5,  0(x1)          // *(x1 + 0) = x5
    sw      x6,  4(x1)          // *(x1 + 4) = x6
    sw      x7,  8(x1)          // *(x1 + 8) = x7
    sw      x8,  12(x1)         // *(x1 + 12) = x8

    lw      x9,  0(x1)          // x9 = *(x1 + 0)
    lw      x10, 4(x1)          // x10 = *(x1 + 4)
    lw      x11, 8(x1)          // x11 = *(x1 + 8)
    lw      x12, 12(x1)         // x12 = *(x1 + 12)

    addi    x1,  x1,    4       // x1 = x1 + 4
    sw      x5,  0(x1)          // *(x1 + 0) = x5
    sw      x6,  4(x1)          // *(x1 + 4) = x6
    sw      x7,  8(x1)          // *(x1 + 8) = x7
    sw      x8,  12(x1)         // *(x1 + 12) = x8

    lw      x13, -4(x1)         // x13 = *(x1 - 4)
    lw      x14, 0(x1)          // x14 = *(x1 + 0)
    lw      x15, 4(x1)          // x15 = *(x1 + 4)
    lw      x16, 8(x1)          // x16 = *(x1 + 8)

    // Calculate a checksum for easy comparison
    add     x17, zero,  x9      // x17 = x9
    add     x17, x17,   x10     // x17 = x17 + x10
    add     x17, x17,   x11     // x17 = x17 + x11
    add     x17, x17,   x12     // x17 = x17 + x12
    add     x17, x17,   x13     // x17 = x17 + x13
    add     x17, x17,   x14     // x17 = x17 + x14
    add     x17, x17,   x15     // x17 = x17 + x15
    add     x17, x17,   x16     // x17 = x17 + x16

    addi    a0,  zero,  0xa     // a0 (x10) = 0xa
    ecall                       // Terminate the simulation by passing 0xa to
                                // ecall in register a0 (x10).
