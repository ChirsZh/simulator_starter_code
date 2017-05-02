# 18-447 RISC-V 32-bit Simulator Makefile
#
# ECE 18-447
# Carnegie Mellon University
#
# This Makefile compiles the simulator and assembles test into RISC-V code for
# the simulator.
#
# Authors:
#	- 2016: Brandon Perez

################################################################################
#                           DO NOT MODIFY THIS FILE!                           #
#                  You should only add files or change sim.c!                  #
################################################################################

################################################################################
# General Targets and Variables
################################################################################

# Set the shell to bash for when the Makefile runs shell commands.
SHELL = /bin/bash -o pipefail

# Terminal color and modifier attributes
# Return to the normal terminal colors
n := $(shell tput sgr0)
# Red color
r := $(shell tput setaf 1)
# Green color
g := $(shell tput setaf 2)
# Bold text
b := $(shell tput bold)
# Underlined text
u := $(shell tput smul)

# These targets don't correspond to actual generated files
.PHONY: all default clean veryclean check-test-defined check-tests-defined

# By default, compile the simulator
all default: help

# Cleanup the intermediate files generated by compiling the simulator
clean: build-clean verify-clean

# Cleanup the intermediate files generated by assembling test programs
veryclean: clean assemble-veryclean run-veryclean

# Check that the TEST variable was specified by the user
check-test-defined:
ifeq ($(strip $(TEST)),)
	@printf "$rError: Variable $bTEST$n$r was not specified.\n$n"
	@exit 1
endif

# Check that the TESTS variable was specified
check-tests-defined:
ifeq ($(strip $(TESTS)),)
	@printf "$rError: Variable $bTESTS$n$r was not specified.\n$n"
	@exit 1
endif

################################################################################
# Assemble Test Programs
################################################################################

# These targets don't correspond to actual files
.PHONY: assemble assemble-veryclean assemble-check-test assemble-check-objcopy \
		assemble-check-objdump assemble-check-compiler

# Prevent make from automatically deleting the generated intermediate ELF file.
.SECONDARY:

# The name of the entry point for assembly tests, which matches the typical main
RISCV_ENTRY_POINT = main

# The addresses of the data and text sections in the program
RISCV_TEXT_START = 0x00400000
RISCV_DATA_START = 0x10000000

# The compiler for assembly files, along with its flags
RISCV_CC = riscv64-unknown-elf-gcc
RISCV_CFLAGS = -static -nostdlib -nostartfiles -m32 -march=RV32IM -Wall \
		-Wextra  -std=c11 -pedantic -g -Werror=implicit-function-declaration
RISCV_AS_LDFLAGS = -Wl,-e$(RISCV_ENTRY_POINT)
RISCV_LDFLAGS = -Wl,--section=.text=$(RISCV_TEXT_START) \
		-Wl,--section=.data=$(RISCV_DATA_START)

# The objcopy utility for ELF files, along with its flags
RISCV_OBJCOPY = riscv64-unknown-elf-objcopy
RISCV_OBJCOPY_FLAGS = -O binary

# The objdump utility for ELF files, along with its flags
RISCV_OBJDUMP = riscv64-unknown-elf-objdump
RISCV_OBJDUMP_FLAGS = -d -M numeric,no-aliases

# The runtime environment directory, which has the startup file for C programs
447_RUNTIME_DIR = 447runtime
RISCV_STARTUP_FILE = $(447_RUNTIME_DIR)/crt0.S

# The file extensions for all files generated, including intermediate ones
ELF_EXTENSION = elf
BINARY_EXTENSION = bin
DISAS_EXTENSION = disassembly.s

# The binary files generated when the program is assembled. There's one for each
# assembled segment: user and kernel text and data sections.
TEST_NAME = $(basename $(TEST))
BINARY_SECTIONS = $(addsuffix .$(BINARY_EXTENSION),text data ktext kdata)
TEST_BIN = $(addprefix $(TEST_NAME).,$(BINARY_SECTIONS))

# The ELF and disassembly files generated when the test is assembled
TEST_EXECUTABLE = $(addsuffix .$(ELF_EXTENSION), $(TEST_NAME))
TEST_DISASSEMBLY = $(addsuffix .$(DISAS_EXTENSION), $(TEST_NAME))

# Assemble the program specified by the user on the command line
assemble: $(TEST) $(TEST_BIN) $(TEST_DISASSEMBLY) | check-test-defined

# Extract the given section from the program ELF file, generating a binary
$(TEST_NAME).%.$(BINARY_EXTENSION): $(TEST_EXECUTABLE) | assemble-check-objcopy
	@$(RISCV_OBJCOPY) $(RISCV_OBJCOPY_FLAGS) -j .$* $^ $@

# Generate a disassembly of the compiled program for debugging proposes
%.$(DISAS_EXTENSION): %.$(ELF_EXTENSION) | assemble-check-objdump
	@$(RISCV_OBJDUMP) $(RISCV_OBJDUMP_FLAGS) $^ > $@
	@printf "Assembly of the test has completed. A disassembly of the test can "
	@printf "be found at $u$*.$(DISAS_EXTENSION)$n.\n"

# Compile the assembly test program with a *.s extension to create an ELF file
%.$(ELF_EXTENSION): %.s | assemble-check-compiler assemble-check-test
	@printf "Assembling test $u$<$n into binary files...\n"
	@$(RISCV_CC) $(RISCV_CFLAGS) $(RISCV_LDFLAGS) $(RISCV_AS_LDFLAGS) $^ -o $@

# Compile the assembly test program with a *.S extension to create an ELF file
%.$(ELF_EXTENSION): %.S | assemble-check-compiler assemble-check-test
	@printf "Assembling test $u$<$n into binary files...\n"
	@$(RISCV_CC) $(RISCV_CFLAGS) $(RISCV_LDFLAGS) $(RISCV_AS_LDFLAGS) $^ -o $@

# Compile the C test program with the startup file to create an ELF file
%.$(ELF_EXTENSION): $(RISCV_STARTUP_FILE) %.c | assemble-check-compiler \
		assemble-check-test
	@printf "Assembling test $u$<$n into binary files...\n"
	@$(RISCV_CC) $(RISCV_CFLAGS) $(RISCV_LDFLAGS) $^ -o $@

# Checks that the given test exists. This is used when the test doesn't have
# a known extension, and suppresses the 'no rule to make...' error message
$(TEST): assemble-check-test

# Clean up all the binary files in project directories
assemble-veryclean:
	@printf "Cleaning up assembled binary files in the project directory...\n"
	@rm -f $$(find -name '*.$(BINARY_EXTENSION)' -o -name '*.$(ELF_EXTENSION)' \
			-o -name '*.$(DISAS_EXTENSION)')

# Check that the RISC-V compiler exists
assemble-check-compiler:
ifeq ($(shell which $(RISCV_CC) 2> /dev/null),)
	@printf "$rError: $u$(RISCV_CC)$n$r: RISC-V compiler was not found in "
	@printf "your PATH.$n\n"
	@exit 1
endif

# Check that the specified test file exists
assemble-check-test:
ifeq ($(wildcard $(TEST)),)
	@printf "$rError: $u$(TEST)$n$r: RISC-V test file does not exist.$n\n"
	@exit 1
endif

# Check that the RISC-V objcopy binary utility exists
assemble-check-objcopy:
ifeq ($(shell which $(RISCV_OBJCOPY) 2> /dev/null),)
	@printf "$rError: $u$(RISCV_OBJCOPY)$n$r: RISC-V objcopy binary utility "
	@printf "was not found in your PATH.$n\n"
	@exit 1
endif

# Check that the RISC-V objdump binary utility exists
assemble-check-objdump:
ifeq ($(shell which $(RISCV_OBJDUMP) 2> /dev/null),)
	@printf "$rError: $u$(RISCV_OBJDUMP)$n$r: RISC-V objdump binary utility "
	@printf "was not found in your PATH.$n\n"
	@exit 1
endif

################################################################################
# Compile the Simulator
################################################################################

# These targets don't correspond to actual files
.PHONY: build build-clean build-check-readline

# The compiler for the simulator, along with its flags
SIM_CC = gcc
SIM_CFLAGS = -Wall -Wextra -std=gnu11 -pedantic -g \
		-Werror=implicit-function-declaration
SIM_INC_FLAGS = -I $(447INCLUDE_DIR)

# The flags for linking against the readline library
LIBREADLINE_FLAGS = -l readline

# The directory for starter code files provided by the 18-447 staff, and all
# the *.c and *.h files in it, and the include directory for header files
447SRC_DIR = 447src
447INCLUDE_DIR = 447include
447SRC = $(shell find $(447SRC_DIR) $(447INCLUDE_DIR) -type f -name '*.c' \
		-o -name '*.h')

# The directory for student source files, and *.c and *.h files in it
SRC_DIR = src
SRC = $(shell find $(SRC_DIR) -type f -name '*.c' -o -name '*.h')

# The name of the executable generated by compiling the simulator
SIM_EXECUTABLE = riscv-sim

# User-facing target to compile the simulator into an executable
build: $(SIM_EXECUTABLE)

# Compile the simulator into an executable
$(SIM_EXECUTABLE): $(SRC) $(447SRC) | build-check-readline
	@printf "Compiling the simulator into an executable...\n"
	@$(SIM_CC) $(SIM_CFLAGS) $(SIM_INC_FLAGS) $(filter %.c,$^) -o $@ \
			$(LIBREADLINE_FLAGS)
	@printf "Compilation of the simulator has completed. The simulator can be "
	@printf "found at $u$@$n.\n"

# Cleanup any intermediate files generated by compiling the simulator
build-clean:
	@printf "Cleaning up the simulator files...\n"
	@rm -f $(SIM_EXECUTABLE)

# Checks that the readline library is installed on the system
build-check-readline:
ifeq ($(shell ldconfig -p | grep "libreadline\.so"),)
	@printf "$rError: $ulibreadline.so$n$r: Readline library was not found on "
	@printf "in your system.$n\n"
	@exit 1
endif

################################################################################
# Run the Simulator
################################################################################

# These targets don't correspond to actual files
.PHONY: run run-veryclean

# Run the simulator with the specified test
run: $(SIM_EXECUTABLE) $(TEST) | assemble check-test-defined
	@printf "Running test $u$(TEST)$n...\n"
	@./$(SIM_EXECUTABLE) $(TEST)

# Cleanup the history file kept around by the simulator's readline
run-veryclean:
	@rm -f .riscv_sim_history

################################################################################
# Verify Simulator
################################################################################

# Always run the simulator to generate the given test, because the specified
# test can change
.PHONY: verify verify-single $(SIM_REGDUMP) verify-clean \
		verify-check-ref-regdump

# The script used to verify, and the options for it
VERIFY_SCRIPT = sdiff
VERIFY_OPTIONS = --ignore-all-space --ignore-blank-lines

# The reference register dump used to verify the simulator's
REF_REGDUMP = $(basename $(TEST)).reg

# The register dump file generated by running the processor simulator
SIM_REGDUMP = simulation.reg

# Verify that the simulator's register dump for the given tests(s) matches the
# reference register dump for each one.
verify: | check-tests-defined
	@for test in $(TESTS); do \
		make verify-single TEST=$${test}; \
	done

# Verify the simulator's register dump for a single test
verify-single: $(SIM_REGDUMP) $(REF_REGDUMP) | assemble verify-check-ref-regdump
	@printf "\n"
	@if $(VERIFY_SCRIPT) $(VERIFY_OPTIONS) $^ &> /dev/null; then \
		printf "$gCorrect! The simulator register dump matches the "; \
		printf "reference.$n\n"; \
	else \
		printf "$u$(SIM_REGDUMP):$n\t\t\t\t\t$u$(REF_REGDUMP):$n\n"; \
		$(VERIFY_SCRIPT) $(VERIFY_OPTIONS) $^; \
		printf "$rIncorrect! The simulator register dump does not match the "; \
		printf "reference.$n\n"; \
	fi

# Run the simulator with the given test, generating a register dump
$(SIM_REGDUMP): $(TEST_BIN) $(SIM_EXECUTABLE) $(TEST)
	@printf "Simulating test $u$(TEST)$n...\n"
	@printf "go\nrdump $@\n" | ./$(SIM_EXECUTABLE) $(TEST)

# Suppresses 'no rule to make...' error when the REF_REGDUMP doesn't exist
$(REF_REGDUMP):

# Cleanup any intermediate files generated by running verification
verify-clean:
	@rm -f $(SIM_REGDUMP)

# Check that the reference register dump for the specified test exists
verify-check-ref-regdump:
ifeq ($(wildcard $(REF_REGDUMP)),)
	@printf "$rError: $u$(REF_REGDUMP)$n$r: Reference register dump for test "
	@printf "$u$(TEST)$n$r does not exist.\n$n"
	@exit 1
endif

################################################################################
# Help Target
################################################################################

# These targets don't correspond to actual generated files
.PHONY: help

# Display a help message about how to use the Makefile to the user
help:
	@printf "18-447 Makefile: Help\n"
	@printf "\n"
	@printf "$bUsage:$n\n"
	@printf "\tmake $utarget$n [$uvariable$n ...]\n"
	@printf "\n"
	@printf ""
	@printf "$bTargets:$n\n"
	@printf "\t$bbuild$n\n"
	@printf "\t    Compiles the simulator scode in $u$(SRC_DIR)$n directory\n"
	@printf "\t    into an executable. Generates an executable at\n"
	@printf "\t    $u$(SIM_EXECUTABLE)$n.\n"
	@printf "\n"
	@printf "\t$bassemble$n\n"
	@printf "\t    Assembles the specified $bTEST$n program into binary files\n"
	@printf "\t    for each code section. The binary files are placed in the\n"
	@printf "\t    test's directory under $u<test_name>.<section>.bin$n.\n"
	@printf "\t    A dissabmely of the compiled test is created at\n"
	@printf "\t    $u<test_name>.$(DISAS_EXTENSION)$n.\n"
	@printf "\n"
	@printf "\t$brun$n\n"
	@printf "\t    Invoke the simulator riscv-sim on the specified $bTEST$n\n"
	@printf "\t    program. Builds the simulator and assembles the program\n "
	@printf "\t    $bTEST$n as necessary.\n"
	@printf "\n"
	@printf "\t$bverify$n\n"
	@printf "\t    Runs and verifies all of the tests specified by $bTESTS$n.\n"
	@printf "\t    Takes the same steps as the $bsim$n target and then\n"
	@printf "\t    compares simulation's register dump against the reference.\n"
	@printf "\n"
	@printf "\t$bclean$n\n"
	@printf "\t    Cleans up the files generated by compilation.\n"
	@printf "\n"
	@printf "\t$bveryclean$n\n"
	@printf "\t    Takes the same steps as the $bclean$n target and also\n"
	@printf "\t    cleans up all binary files and disassembly generated from\n"
	@printf "\t    assembling the tests in the project directory.\n"
	@printf "\n"
	@printf "$bVariables:$n\n"
	@printf "\t$bTEST$n\n"
	@printf "\t    The program to assemble. This is a single RISC-V assembly\n"
	@printf "\t    or C file.\n"
	@printf "\n"
	@printf "\t$bTESTS$n\n"
	@printf "\t    A list of programs to verify processor simulation with.\n"
	@printf "\t    This is only used for the 'verify' target. The variable\n"
	@printf "\t    supports glob patterns, a list when quoted, or a single\n"
	@printf "\t    program.\n"
	@printf "\n"
	@printf "$bExamples:$n\n"
	@printf "\tmake build\n"
	@printf "\tmake assemble TEST=inputs/mytest.S\n"
	@printf "\tmake run TEST=inputs/mytest.S\n"
	@printf "\tmake verify TESTS=inputs/mytest.S\n"
	@printf "\tmake verify TESTS=\"inputs/mytest1.S inputs/mytest2.S\"\n"
	@printf "\tmake verify TESTS=447inputs/*.S\n"
