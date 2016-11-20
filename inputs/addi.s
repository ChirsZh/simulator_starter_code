# addi.s
#
# Addi Instruction test
#
# This test simply tests that the addi instruction is working as expected.

    .text                       # Declare the code to be in the .text segment
    .global main                # Make main visible to the linker
main:
    addi t0, zero, 10           # t0 (x5) = 0 + 10
    addi t1, zero, 5            # t1 (x6) = 0 + 5
    addi t2, t0, 300            # t2 (x7) = t1 + 300
    addi t3, zero, 500          # t3 (x28) = 0 + 500
    addi t4, t3, 34             # t4 (x29) = t3 + 34
    addi t4, t4, 45             # t4 (x29) = t4 + 45

    ret                         # Return to the startup code
