# brtest0.s
#
# Basic Branch Test
#
# This is a very basic branch test, that only tests unconditional jumps, and
# branches that are trivially true or false.

    .text                           # Declare the code to be in the .text segment
    .global main                    # Make main visible to the linker
main:
    addi    a0,   zero,  0xa        # a0 (x10) = 0xa

label0:                             # Should reach here (1.)
    jal     zero, label1            # Goto label1 (no link)

label1:                             # Should reach here (2.)
    bne     zero, zero,  label3     # If (0 != 0) goto label3

label2:                             # Should reach here (3.)
    beq     zero, zero,  label4     # If (0 == 0) goto label4
    addi    x7,   zero,  0x347      # x7 = 0x347
    ecall                           # Terminate simulation (should not reach here)

label3:                             # Should not reach here
    addi    x6,   zero,  0x337      # x6 = 0x337

label4:                             # Should reach here (4.)
    addi    x7,   zero,  0x70d      # x7 = 0x70d
    ecall                           # Terminate simulation (should reach here)
