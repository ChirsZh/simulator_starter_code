# addi.s
#
# Addi Instruction test
#
# This test simply tests that the addi instruction is working as expected.

    .text                       # Declare the code to be in the .text segment
    .global main                # Make main visible to the linker
main:
    addi x2, zero, 10           # x2 = 0 + 10
    addi t0, zero, 5            # t0 (x12) = 0 + 5
    addi t1, t0, 300            # t1 (x13) = t0 + 300
    addi t2, zero, 500          # t2 (x14) = 0 + 500
    addi t3, t2, 34             # t3 (x15) = t2 + 34
    addi t3, t3, 45             # t3 (x15) = t3 + 45

    ret                         # Return to the startup code
