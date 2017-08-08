# arithtest.S
#
# Basic Arithmetic Instructions Test
#
# This tests a subset of the arithmetic instructions in RISC-V to test basic
# functionality. This test also demonstrates how the ISA names names for
# registers can be used, as opposed to their ABI names (e.g. x2 vs. ra).

    .text                       # Declare the code to be in the .text segment
    .global main                # Make main visible to the linker
main:
    addi    x2,  x0,  1024      # x2 = 1024
    add     x3,  x2,  x2        # x3 = x2 + x2
    or      x4,  x3,  x2        # x4 = x3 | x2
    add     x5,  x0,  1234      # x5 = 1234
    slli    x6,  x5,  16        # x6 = x5 << 16
    addi    x7,  x6,  999       # x7 = x6 + 999
    sub     x8,  x7,  x2        # x8 = x7 - x2
    xor     x9,  x4,  x3        # x9 = x4 ^ x3
    xori    x10, x2,  255       # x10 = x2 ^ 255
    srli    x11, x6,  5         # x11 = x6 >> 5 (logical)
    srai    x12, x6,  4         # x12 = x6 >> 4 (arithmetic)
    and     x13, x11, x7        # x13 = x11 & x7
    andi    x14, x5,  100       # x14 = x5 & 100
    sub     x15, x0,  x10       # x15 = -x10
    lui     x16, 100            # x16 = 100 << 12

    addi    x10, x0, 0xa        # x10 (a0) = 0xa
    ecall                       # Terminate the simulation by passing 0xa to
                                # ecall in register a0 (x10).
