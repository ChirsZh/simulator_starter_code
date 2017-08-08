# shifttest.S
#
# Basic Shift Instructions Test
#
# This performs a basic test of the shift instructions with immediate values.

    .text                       # Declare the code to be in the .text segment
    .global main                # Make main visible to the linker
main:
    ori     x3, zero, -1043     # x3 = 0xfffffbed
    slli    x4, x3, 21          # x4 = x3 << 21
    slli    x5, x3, 6           # x5 = x3 << 6
    srai    x6, x4, 10          # x6 = x4 >> 10 (arithmetic)
    srai    x7, x5, 16          # x7 = x5 >> 16 (arithmetic)
    srli    x8, x4, 10          # x8 = x4 >> 10 (logical)
    srli    x9, x5, 16          # x9 = x5 >> 16 (logical)

    addi    a0, zero, 0xa       # a0 (x10) = 0xa
    ecall                       # Terminate the simulation by passing 0xa to
                                # ecall in register a0 (x10).
