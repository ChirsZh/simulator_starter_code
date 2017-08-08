# brtest1.S
#
# Basic Branch Test
#
# This is a very basic branch test, that only tests unconditional jumps, and
# branches that are trivially true or false.

    .text                           # Declare the code to be in .text section
    .global main                    # Make main visible to the linker
main:
    addi    a0,   zero,  0xa        # a0 (x10) = 0xa

label0:                             # Should reach here (1.)
    addi    x5,   zero,  1          # x5 = 1
    jal     zero, label1            # Goto label1 (no link)

    addi    x10,  x10,   0x700      # x10 = x10 + 0x700
    ori     x0,   x0,    0          # No-op
    ori     x0,   x0,    0          # No-op
    addi    x5,   zero,  100        # x5 = 100
    ecall                           # Terminate simulation (should not reach)

label1:                             # Should reach here (2.)
    bne     zero, zero,  label3     # If (0 != 0) goto label3
    ori     x0,   x0,    0          # No-op
    ori     x0,   x0,    0          # No-op
    addi    x6,   zero,  1337       # x6 = 1337

label2:                             # Should reach here (3.)
    beq     zero, zero,  label4     # If (0 == 0) goto label4
    ori     x0,   x0,    0          # No-op
    ori     x0,   x0,    0          # No-op
    addi    x7,   zero,  0x347      # x7 = 0x347
    ecall                           # Terminate simulation (should not reach)

label3:                             # Should not reach here
    addi    x8,   zero,  0x404      # x8 = 0x3404
    ecall                           # Terminate simulation (should not reach)

label4:                             # Should reach here
    addi    x7,   zero,  0x447      # x7 = 0x447
    ecall                           # Terminate simulation (should reach here)
