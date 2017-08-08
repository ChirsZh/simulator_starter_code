# syscalltest.S
#
# Syscall instruction test
#
# This test verifies that the ECALL instruction only halts simulation when
# register x10 has value 0x0A

    .text                       # Declare the code to be in the .text segment
    .global main                # Make main visible to the linker
main:
    addi    t0, zero, 10        # t0 (x5) = 10
    addi    a0, zero, 0         # a0 (x10) = 0
    ecall                       # Shouldn't do anything, since x10 != 0x0A
    addi    a0, zero, 0xa       # a0 (x10) = 0xa
    ecall                       # Terminate the simulation
