# brtest2.S
#
# Advanced Branch Test
#
# This tests all of the control flow instructions in the RISC-V ISA. This tests
# all of the signed branch instructions, including reversing the operands to
# achieve branches that aren't explicitly in the RISC-V ISA). For example, to
# perform a less than or equal to branch, you can reverse the operands of a
# a BGE instruction.

    .text                           # Declare the code to be in .text section
    .global main                    # Make main visible to the linker
main:
    addi    a0,   zero,   0xa       # a0 (x10) = 0xa

    addi    x3,   zero,   1         # x3 = 1
    addi    x4,   zero,   -1        # x4 = -1
    addi    x5,   zero,   0x123     # x5 = 0x123 (Checksum register)

    jal     zero, label1            # Goto label1, no link

label0:                             # Should reach here (2.)
    add     x5,   x5,     ra        # x5 = x5 + ra (x1)
    beq     zero, zero,   label2    # if (0 == 0) goto label2

label1:                             # Should reach here (1.)
    addi    x5,   x5,     7         # x5 = x5 + 7
    jal     ra,   label0            # Goto label0, ra (x1) = pc + 4
    jal     zero, label8            # Goto label8, no link

label2:                             # Should reach here (3.)
    addi    x5,   x5,     9         # x5 = x5 + 9
    bne     x3,   x4,     label4    # if (x3 != x4) goto label4

label3:                             # Should reach here (6.)
    addi    x5,   x5,     5         # x5 = x5 + 5
    bge     zero, zero,   label6    # if (0 >= 0) goto label6

label4:                             # Should reach here (4.)
    addi    x5,   x5,     11        # x5 = x5 + 11
    bge     zero, x3,     label3    # if (x3 <= 0) goto label3

label5:                             # Should reach here (5.)
    addi    x5,   x5,     99        # x5 = x5 + 99
    blt     zero, x3,     label3    # if (x3 > 0) goto label3

label6:                             # Should reach here (7.)
    addi    x5,   x5,     111       # x5 = x5 + 111
    jalr    zero, ra,     0         # Return to label1 (no link)

label7:                             # Should not reach here
    addi    x5,   x5,     200       # x5 = x5 + 200
    ecall                           # Terminate simulation (should not reach)

label8:                             # Should reach here (8.)
    addi    x5,   x5,     215       # x5 = x5 + 215
    jal     ra,   label10           # Goto label10, ra (x1) = pc + 4

label9:                             # Should not reach here
    addi    x5,   x5,     1         # x5 = x5 + 1
    ecall                           # Terminate simulation (should not reach)

label10:                            # Should reach here (9.)
    addi    x5,   x5,     447       # x5 = x5 + 447
    blt     x4,   zero,   label12   # if (x4 < 0) goto label12

label11:                            # Should not reach here
    addi    x5,   x5,     400       # x5 = x5 + 400
    ecall                           # Terminate simulation (should not reach)

label12:                            # Should reach here (10.)
    add     x5,   x5,     ra        # x5 = x5 + ra (x1)
    bge     x4,   zero,   label11   # if (x4 >= 0) goto label11

label13:                            # Should reach here (11.)
    addi    x5,   x5,     0x63d     # x5 = x5 + 0x63d
    ecall                           # Terminate simulation (should reach here)
