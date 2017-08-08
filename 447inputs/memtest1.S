# memtest1.S
#
# Advanced Memory Test
#
# This performs a more complete test of the load and instructions, checking
# that sub-word (e.g. byte and half-word) instructions work. This also tests
# that the processor is respecting the little-endian memory ordering mandated
# by the RISC-V ISA.

    .data                       # Declare items to be in the .data segment
data:                           # Symbol representing the start of .data
    .space 20                   # Allocate 20 bytes for the .data segment
data_end:                       # Symbol representing the end of .data

    .text                       # Declare the code to be in the .text segment
    .global main                # Make main visible to the linker
main:
    addi    x5,  zero, 0x7de    # x5 = 0x7de
    addi    x6,  zero, 0x5ca    # x6 = 0x5ca
    addi    x7,  zero, 0x33f    # x7 = 0x33f
    lui     x8,  0xd            # x8 = 0xd000
    addi    x8,  x8,   -1282    # x8 = x8 + 0xfffffafe (x8 = 0xcafe)

    sb      x5,  0(gp)          # *(gp + 0) = x5
    sb      x6,  1(gp)          # *(gp + 1) = x6
    sb      x7,  6(gp)          # *(gp + 6) = x7
    sb      x8,  7(gp)          # *(gp + 7) = x8

    lbu     x9,  0(gp)          # x9 = zero_extend(*gp + 0)
    lbu     x10, 1(gp)          # x10 = zero_extend(*gp + 0)
    lb      x11, 6(gp)          # x11 = sign_extend(*gp + 0)
    lb      x12, 7(gp)          # x12 = sign_extend(*gp + 0)

    addi    gp,  gp,   4        # gp (x3) = gp + 4
    sh      x5,  0(gp)          # *(gp + 0) = x5
    sh      x8,  2(gp)          # *(gp + 2) = x6
    sh      x7,  4(gp)          # *(gp + 4) = x7
    sh      x8,  6(gp)          # *(gp + 6) = x8

    lhu     x13, 0(gp)          # x13 = zero_extend(gp + 0)
    lhu     x14, 2(gp)          # x14 = zero_extend(gp + 2)
    lh      x15, 4(gp)          # x15 = sign_extend(gp + 4)
    lh      x16, 6(gp)          # x16 = sign_extend(gp + 6)

    # Calculate a checksum for easy comparison
    add     x17, zero, x9       # x17 = x9
    add     x17, x17,  x10      # x17 = x17 + x10
    add     x17, x17,  x11      # x17 = x17 + x11
    add     x17, x17,  x12      # x17 = x17 + x12
    add     x17, x17,  x13      # x17 = x17 + x13
    add     x17, x17,  x14      # x17 = x17 + x14
    add     x17, x17,  x15      # x17 = x17 + x15
    add     x17, x17,  x16      # x17 = x17 + x16

    addi    a0,  zero, 0xa      # a0 (x10) = 0xa
    ecall                       # Terminate the simulation by passing 0xa to
                                # ecall in register a0 (x10).
