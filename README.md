# 18-447 RISC-V Processor C Simulator (Lab 1a)

## Getting Started

### Machines

The build system and code for the 18-447 labs will only run on ECE machines, as they are dependent on tools that can't
be installed locally on your machine (e.g. VCS). Thus, you will only be able to run the labs on one of the lab machines
in Hamerschlag Hall 1305 or one of the ECE servers. These servers are given by ecexxx.ece.cmu.edu, where xxx is between
000 and 031.

These ECE servers are only accessible if you are physically on CMU's campus network (they are **not** accessible through
the CMU VPN). If you are off campus, then you can use one of the public ECE servers, which do not require a VPN. These
servers are given by ece-gui-xxx.ece.cmu.edu, where xxx is between 004 and 007. Note that there are also 000 through 003
servers available.  However, the RISC-V cross-compiler requires a newer version of glibc than is present on these
servers, so the lab code cannot be run on these machines. See the
[ECE IT User Guide](https://userguide.its.cit.cmu.edu/resources/computer-clusters/) for more information on the computer
clusters.

### Setting Up the 18-447 Tools and Build Environment

All of the 18-447 tools and scripts are located under **/afs/ece/class/ece447/bin/**. Before you can compile and run any
of your code, you must setup your environment variables. To setup the build environment for the labs, run:

```bash
source /afs/ece/class/ece447/bin/447setup
```

This sets up your environment variables properly so that you can run the VCS compiler for simulation, the DC compiler
for synthesis, the RISC-V cross-compiler for compiling test programs, and the reference RISC-V simulator. It is
recommended that you put this command in your **.bashrc** file, so that the build environment is setup automatically
every time you log into a machine. To do this, run:

```bash
printf "source /afs/ece/class/ece447/bin/447setup\n" >> ${HOME}/.bashrc
```

### Starter Code and Requirements

To get started, you are provided code that will run the `addi` and `add` instructions. This is located in the file
**[src/sim.c](src/sim.c)**. You are free to add, change, and delete any files under the **src** directory, but you are
not allowed to modify any files outside the **src** directory. The only requirement is that you have a
`process_instruction` function defined with the following signature:

```c
void process_instruction(cpu_state_t *cpu_state)
```

This function will be called by the simulator each time it needs to simulate a single processor cycle. In the
single-cycle design that you will be simulating, this corresponds to processing the next instruction. This function
should update the register values, memory, and PC register as required by the next instruction in the program.

The build system will automatically discover any new files you add under the **src** directory, provided that they have
a *.c* or *.h* extension. The files may be nested in any subdirectories under the **src** directory. Additionally, the
build system sets up the include paths so that you can place header files in any subdirectory under the **src**
directory as well, and include them from anywhere inside the **src** directory.

You are only required to implement part of the RV32I (32-bit integer) subset of the RISC-V ISA. See the current [RISC-V
ISA specification](https://riscv.org/specifications/) for the details on each instruction. The RV32I Base Integer
Instruction Set chapter will contain the specifications for the behavior of each instruction that your processor will
need to support. Additionally, see the RISC-V Assembly Programmer's Handbook chapter for the application binary
interface (ABI) and assembler pseudoinstructions.

## Running Simulation

### Running the C Simulator

To run the C simulator, you can run the following command:

```bash
make run TEST=<path/to/test>
```

Naturally, you should replace **<path/to/test>** with the actual path to the test you want to run (e.g.
**447inputs/additest.S**). This will build the C simulator into an executable at **riscv-sim**, assemble the specified
test, and then run the simulator, dropping you into the simulator shell. To see the available commands in the simulator
shell, run `?`, `h`, or `help`.

### Verifying Your Simulator

To verify that your simulator produces the correct results for a given test, you can run:

```bash
make verify TEST=<path/to/test>
```

This will take the same steps as the *run* target, except instead of dropping you into the simulator shell, it will run
the test to completion and produce a register dump. It will then compare this register dump to the test's reference
register dump (e.g. **447inputs/additest.reg**), and notify you if your dump differs from it.

You can also run verification against a suite or batch of tests. For example, to run verification with all tests with a
*.S* extension under the **447inputs** directory, you can run:

```bash
make autograde TESTS=447inputs/*.S
```

In this case, the Makefile only prints out a summary for each test saying whether it passed or failed, and the output of
each individual test is suppressed. The **TESTS** variable is optional. If it is left unspecified, then it defaults to
the set of tests that you are required to pass for checkoff for this lab. So, if you want to see if you are ready for
checkoff, run:

```bash
make autograde
```

### Other Makefile Commands

For a complete listing of the Makefile commands and variables, run:

```bash
make help
```

## Debugging Your Simulator

### Test Disassembly Files

When the build system compiles and assembles a test, it also generates a disassembly file for the test. This is useful
when debugging tests as the disassembly shows each instruction along with its address in memory. Naturally, this is
particularity useful when debugging C tests. The disassembly will also show the values of any global variables defined
in the program. The disassembly file will be located at **<test_name>.disassembly.s** (e.g.
**447inputs/additest.disassembly.s**).

### Simulator Commands

There are several simulator commands to help you debug your implementation. The `reg` command allows you to either
display the value of a specific register, or to update the register with a value. You can use the ISA name (e.g. *x2*),
ABI name (e.g. *sp*), or simply a number (e.g. 2) to refer to the register. The `rdump` command displays all the
register values, along with the CPU state. Optionally, you can specify a file to which to write the register dump, which
is how you can generate the reference register dumps, or *.reg* files.

There are also commands to view memory. The `mem` command allows you to either display the value of a memory location,
or update that address with a value. The address can be specified as either a hexadecimal or decimal value. The `mdump`
command displays a range of memory values. Optionally, you can specify a file to which to write the memory dump.

To see a complete listing of the available commands, run the `?`, `h`, or `help` commands.

### Reference Simulator and Verbose Mode

There is a "golden" reference simulator that you can use to help with debugging. The reference simulator is located at
**/afs/ece/class/ece447/bin/riscv-ref-sim**. If you have sourced the 18-447 setup script, then you can simply run:

```bash
riscv-ref-sim </path/to/test>
```

The simulator supports a verbose mode, which can be toggled off and on with the `verbose` command. In verbose mode, the
simulator prints out a register dump after every cycle that the simulator runs. This can be useful for performing a
cycle-by-cycle comparison with the reference simulator. You could perform a cycle-by-cycle comparison as follows:

```bash
printf "verbose\ngo\n" | ./riscv-sim </path/to/test> &> sim.log
printf "verbose\ngo\n" | riscv-ref-sim </path/to/test> &> ref.log
diff -w -B sim.log ref.log | head -n 10
```

Then, you can use the line number outputted by `diff`, and go back into either one of the logs, and figure out which
cycle your simulator started differing from the reference simulator.

## Writing Your Own Tests

### Writing Tests

It is **strongly** recommended that your write your own tests, because the public tests provided by the 18-447 staff are
not comprehensive. The build system supports two types of test programs: assembly and C programs. The only requirements
for both types of test programs is that they must contain a function named `main`. The `main` function is the entry
point for your program, which is where execution will start.

### Generating a Register Dump

When running verification, the build system expects that there is a *.reg* file with the same name as the test (e.g.
**[447inputs/additest.reg](447inputs/additest.reg)**). This is the register dump at the end of the program's execution,
and is used as reference to compare with the implementation's register dump. This register dump can be generated by the
`rdump` command in the C simulator. Thus, to generate a register dump, you can use the reference simulator and run:

```bash
printf "go\nrdump <path/to/test_name>.reg\n" | ./riscv-ref-sim <path/to/test>
```

Alternately, you can use your own C simulator to generate the register dump.

### Writing an Assembly Test

All assembly programs must end with a *.S* extension. Since assembly programs have a *.S* extension, the preprocessor is
run on them before they are assembled. Thus, you have access to preprocessor directives (e.g. `#define`, `#ifndef`,
etc.), and so you can use macros in your program.

The main function must be explicitly declared with the `.text` directive so it ends up the text section, and made
visible to the linker with the `.global` directive. To end a test and terminate simulation, the `ecall` instruction must
be invoked with the value of *0xa* in the *a0* (*x10*) register. Thus, the most bare-bones assembly test program would
look like:

```assembly
    .text
    .global main
main:
    addi x10, x0, 0xa
    ecall
```

You can also add memory to your assembly programs, in the data section. This can accomplished with several different
assembler directives. For zero-initialized memory, the `.space` directive can be used. For initializing memory with
specific values, there are various directives, such as `.word`, `.halfword`, etc. The simulator will only allocate as
much memory as you request, so you must explicitly declare how much memory you need in your program. For example, to
allocate 20 bytes of zero-initialized memory in the data section, you would write:

```assembly
    .data
data_start:
    .space 20
data_end:
```

The RISC-V assembly language also supports pseudoinstructions. These are instructions that the assembler recognizes, but
are not in the RISC-V ISA. When the assembler encounters these instructions, it substitutes one or more actual RISC-V
instructions needed to implement the pseudoinstruction. This can make debugging a bit tricky, since in the assembled
program the pseudoinstructions may be represented by several actual instructions; be sure to use the disassembly of the
program when debugging.

For example, there is the `li` pseudoinstruction, which loads a 32-bit immediate into a register. Depending on the value
of the immediate, this instruction can expand to an `lui` followed by an `ori`. For a complete listing of
pseudoinstructions, see the RISC-V Assembly Programmer's Handbook chapter in the
[RISC-V ISA Specification](https://riscv.org/specifications/).

For an example of a basic assembly test, see **[447inputs/addtest.S](447inputs/addtest.S)**. For an example of an
assembly test that uses memory, see **[447inputs/memtest0.S](447inputs/memtest0.S)**. For a complete listing and
description of supported assembler directives, see the
[GNU Assembler (GAS) Guide](https://sourceware.org/binutils/docs/as/Pseudo-Ops.html#Pseudo-Ops).

### Writing a C Test

All C programs must end with a *.c* extension. The only requirement for a C program is that it contains a function named
`main`. The C program is wrapped by a small assembly startup function, located in the file
**[447runtime/crt0.S](447runtime/crt0.S)** (CRT stands for C runtime). This function sets up the environment for the C
program to run, invokes the user program, and, when main returns, invokes the `ecall` instruction as described before to
terminate simulation.

The RISC-V ABI permits doubleword return values. Thus, the lower 32-bits of the user program's return value are placed
in the *x2* (*sp*) register, while the upper 32-bits are placed in the *x3* (*gp*) register.

The C program test runs in a very minimal environment. None of the C standard library functions are available, and no
external functions can be called from the program, other than compiler intrinsic functions. The only functions that can
be called are ones which are defined in the same file. Other than that, all parts of the C language are supported.

Even though the processor only supports the RV32I subset of the RISC-V ISA, it still is possible to use floating-point
and integer multiplication operations from within C programs. The code is linked against the GCC library, which provides
software implementations of these operations. These implementations faithfully emulate floating-point and integer
multiplication operations with only RV32I instructions.

For an example of a C test, see **[447inputs/matrix_mult.c](447inputs/matrix_mult.c)**. This test also utilizes
floating-point values, showing how the *libgcc* functions are compiled into the program.

## Appendix

### Running the Simulator Locally on Your Machine

For this lab only, with the C simulator, it is possible to run all of the build system locally on your machine. To do
so, you simply need to install RISC-V GCC toolchain. For installing the RISC-V GCC toolchain on your machine, see the
[RISC-V GCC Toolchain Installation Manual](https://riscv.org/software-tools/). Naturally, run your code on one of the
ECE servers before submitting it to make sure it will compile and run when the TA's grade it.

### Overview of the Build System

The build system utilizes the RISC-V GCC toolchain to compile and assemble test programs. When a test is run for
simulation, it is first compiled into an ELF executable, **<test_name>.elf**, using RISC-V GCC. A linker script, located
at **[447runtime/test_program.ld](447runtime/test_program.ld)**, is used to map all program memory into 4 distinct
sections. There are two text sections, one for user code (*.text*) and one for kernel code (*.ktext*). The text sections
contain the corresponding code and any read-only global variables. In addition, there are two data sections, one for
user data (*.data*) and one for kernel data (*.kdata*). The data sections contain the corresponding writable global
variables, and any uninitialized global variables (from the *.bss* section).

The simulator expects the 4 sections to be in binary format, in separate files, as parsing an ELF binary is a bit
complex. Thus, the build system utilizes `objcopy` to extract the 4 sections from the ELF binary, placing them in the
corresponding **<test_name>.<section_name>.bin** files. It also concatenates the user and kernel *.bss* sections to the
end of the corresponding *.data* sections into one binary file. A disassembly file for the test also generated from the
ELF executable, under **<test_name>.disassembly.s**.

The build system then compiles the simulator. When the simulator starts, it loads the data for each memory segment from
the corresponding binary files for each program section. In addition, a stack segment, shared between the kernel and
user code, is allocated by the simulator. The simulator also initializes the *sp* (*x2*) register to point to the end of
the stack, and the *gp* (*x3*) register to point to the beginning of the user data segment. Naturally, the *pc* register
is then initialized to point at the beginning of the user text segment, and the program begins execution.

For verification, the build system assumes that there is a register dump under **<test_name>.reg** that has the expected
register state when the program finishes execution. The simulator generates a register dump when the program finishes,
and the build system uses `sdiff` to determine if the two register dumps match.

### Useful Links

[RISC-V ISA Specification](https://riscv.org/specifications/) - [https://riscv.org/specifications/](https://riscv.org/specifications/)

[ECE IT User Guide](https://userguide.its.cit.cmu.edu/resources/computer-clusters/) - [https://userguide.its.cit.cmu.edu/resources/computer-clusters/](https://userguide.its.cit.cmu.edu/resources/computer-clusters/)

[GNU Assembler (GAS) Guide](https://sourceware.org/binutils/docs/as/Pseudo-Ops.html#Pseudo-Ops) - [https://sourceware.org/binutils/docs/as/Pseudo-Ops.html#Pseudo-Ops](https://sourceware.org/binutils/docs/as/Pseudo-Ops.html#Pseudo-Ops)

[RISC-V GCC Toolchain Installation Manual](https://riscv.org/software-tools/) - [https://riscv.org/software-tools/](https://riscv.org/software-tools/)
