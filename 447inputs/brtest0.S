# brtest0.S
#
# Basic Branch Test
#
# This is a very basic branch test, that only tests unconditional jumps, and
# branches that are trivially true or false.

    .text                           # Declare the code to be in .text section
    .global main                    # Make main visible to the linker
main:
    addi    a0,   zero,  0xa        # a0 (x10) = 0xa
    addi    x7,   zero,  0          # x7 = 0x0

label0:
    jal     zero, label1            # Goto label1 (no link)
    addi    x7,   x7,    0x327      # x7 = x7 + 0x327

label1:
    bne     zero, zero,  label3     # If (0 != 0) goto label3

label2:
    beq     zero, zero,  label4     # If (0 == 0) goto label4
    addi    x7,   x7,    0x347      # x7 = x7 + 0x347
    ecall                           # Terminate simulation (should not reach)

label3:
    addi    x7,   x7,    0x337      # x7 = x7 + 0x337

label4:
    addi    x7,   x7,    0x70d      # x7 = x7 + 0x70d
    ecall                           # Terminate simulation (should reach here)
