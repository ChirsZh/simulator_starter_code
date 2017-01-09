# 18-447 RISC-V 32-bit Simulator Makefile
#
# ECE 18-447
# Carnegie Mellon University
#
# This Makefile compiles the simulator and assembles test into RISC-V code for
# the simulator.
#
# Authors:
# 	- 2016: Brandon Perez

################################################################################
#                           DO NOT MODIFY THIS FILE!                           #
#                  You should only add files or change sim.c!                  #
################################################################################

################################################################################
# User Controlled Parameters
################################################################################

# The user can specify the test they want to assemble. Defaults to additest.s
DEFAULT_TEST = 447inputs/additest.s
TEST ?= $(DEFAULT_TEST)

################################################################################
# General Targets and Variables
################################################################################

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
.PHONY: all default clean veryclean

# By default, compile the simulator
all default: help

# Cleanup the intermediate files generated by compiling the simulator
clean: sim-clean

# Cleanup the intermediate files generated by assembling test programs
veryclean: clean assemble-veryclean

################################################################################
# Assemble Test Programs
################################################################################

# These targets don't correspond to actual files
.PHONY: assemble assemble-veryclean

# Prevent make from automatically deleting intermediate files generated.
# Instead, we do this manually. This prevents the commands from being echoed.
.SECONDARY:

# The name of the entrypoint for assembly tests, which matches the typical main
RISCV_ENTRY_POINT = main

# The addresses of the data and text sections in the program
RISCV_TEXT_START = 0x00400000
RISCV_DATA_START = 0x10000000

# The compiler for assembly files, along with its flags
RISCV_CC = riscv64-unknown-elf-gcc
RISCV_CFLAGS = -static -nostdlib -nostartfiles -m32 -Wall -Wextra -std=c11 \
			   -pedantic -g -Werror=implicit-function-declaration
RISCV_AS_LDFLAGS = -Wl,-e$(RISCV_ENTRY_POINT)
RISCV_LDFLAGS = -Wl,--section=.text=$(RISCV_TEXT_START) \
				-Wl,--section=.data=$(RISCV_DATA_START)

# The objcopy utility for ELF files, along with its flags
RISCV_OBJCOPY = riscv64-unknown-elf-objcopy
RISCV_OBJCOPY_FLAGS = -O binary

# The objdump utility for ELF files, along with its flags
RISCV_OBJDUMP = riscv64-unknown-elf-objdump
RISCV_OBJDUMP_FLAGS = -d

# The compiler for hex files, which convert copied binary to ASCII hex files,
# where there is one word per line.
HEX_CC = hexdump
HEX_CFLAGS = -v -e '1/4 "%08x" "\n"'

# The runtime environment directory, which has the startup file for C programs
447_RUNTIME_DIR = 447runtime
RISCV_STARTUP_FILE = $(447_RUNTIME_DIR)/crt0.S

# The file extensions for all files generated, including intermediate ones
ELF_EXTENSION = elf
BINARY_EXTENSION = bin
HEX_EXTENSION = hex
DISAS_EXTENSION = disassembly.s

# The hex files generated when the program is assembled. There's one for each
# assembled segment: user and kernel text and data sections.
TEST_NAME = $(basename $(TEST))
HEX_SECTIONS = $(addsuffix .$(HEX_EXTENSION),text data ktext kdata)
TEST_HEX = $(addprefix $(TEST_NAME).,$(HEX_SECTIONS))

# The ELF and disassembly files generated when the test is assembled
TEST_EXECUTABLE = $(addsuffix .$(ELF_EXTENSION), $(TEST_NAME))
TEST_DISASSEMBLY = $(addsuffix .$(DISAS_EXTENSION), $(TEST_NAME))

# Assemble the program specified by the user on the command line
assemble: $(TEST) $(TEST_HEX) $(TEST_DISASSEMBLY)

# Convert a binary file into an ASCII hex file, with one 4-byte word per line
%.$(HEX_EXTENSION): %.$(BINARY_EXTENSION) | assemble-check-hex-compiler
	@$(HEX_CC) $(HEX_CFLAGS) $^ > $@
	@rm -f $^

# Extract the given section from the program ELF file, generating a binary
$(TEST_NAME).%.$(BINARY_EXTENSION): $(TEST_EXECUTABLE) | assemble-check-objcopy
	@$(RISCV_OBJCOPY) $(RISCV_OBJCOPY_FLAGS) -j .$* $^ $@

# Generate a disassembly of the compiled program for debugging proposes
%.$(DISAS_EXTENSION): %.$(ELF_EXTENSION) | assemble-check-objdump
	@$(RISCV_OBJDUMP) $(RISCV_OBJDUMP_FLAGS) $^ > $@
	@rm -f $^
	@printf "Assembly of the test has completed. A disassembly of the test can "
	@printf "be found at $u$*.$(DISAS_EXTENSION)$n.\n"

# Compile the assembly test program with a *.s extension to create an ELF file
%.$(ELF_EXTENSION): %.s | assemble-check-compiler assemble-check-test
	@printf "Assembling test $u$<$n into hex files...\n"
	@$(RISCV_CC) $(RISCV_CFLAGS) $(RISCV_LDFLAGS) $(RISCV_AS_LDFLAGS) $^ -o $@

# Compile the assembly test program with a *.S extension to create an ELF file
%.$(ELF_EXTENSION): %.S | assemble-check-compiler assemble-check-test
	@printf "Assembling test $u$<$n into hex files...\n"
	@$(RISCV_CC) $(RISCV_CFLAGS) $(RISCV_LDFLAGS) $(RISCV_AS_LDFLAGS) $^ -o $@

# Compile the C test program with the startup file to create an ELF file
%.$(ELF_EXTENSION): %.c $(RISCV_STARTUP_FILE) | assemble-check-compiler \
		assemble-check-test
	@printf "Assembling test $u$<$n into hex files...\n"
	@$(RISCV_CC) $(RISCV_CFLAGS) $(RISCV_LDFLAGS) $^ -o $@

# Checks that the given test exists. This is used when the test doesn't have
# a known extension, and suppresses the 'no rule to make...' error message
$(TEST): assemble-check-test

# Clean up all the hex files in project directories
assemble-veryclean:
	@printf "Cleaning up all assembled hex files in the project directory...\n"
	@rm -f $$(find -name '*.$(HEX_EXTENSION)' -o -name '*.$(BINARY_EXTENSION)' \
			-o -name '*.$(ELF_EXTENSION)' -o -name '*.$(DISAS_EXTENSION)')

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

# Check that the hex compiler exists (converts binary to ASCII hex)
assemble-check-hex-compiler:
ifeq ($(shell which $(HEX_CC) 2> /dev/null),)
	@printf "$rError: $u$(HEX_CC)$n$r: Hex dump utility was not found in your "
	@printf "PATH.\n"
	@exit 1
endif

################################################################################
# Compile the Simulator
################################################################################

# These targets don't correspond to actual files
.PHONY: sim sim-clean

# The compiler for the simulator, along with its flags
SIM_CC = gcc
SIM_CFLAGS = -Wall -Wextra -std=gnu11 -pedantic -g \
			 -Werror=implicit-function-declaration
SIM_INC_FLAGS = -I $(447INCLUDE_DIR)

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
$(SIM_EXECUTABLE): $(SRC) $(447SRC)
	@printf "Compiling the simulator into an executable...\n"
	@$(SIM_CC) $(SIM_CFLAGS) $(SIM_INC_FLAGS) $(filter %.c,$^) -o $@
	@printf "Compilation of the simulator has completed. The simulator can be "
	@printf "found at $u$@$n.\n"

# Cleanup any intermediate files generated by compiling the simulator
sim-clean:
	@printf "Cleaning up the simulator files...\n"
	@rm -f $(SIM_EXECUTABLE)

################################################################################
# Run the Simulator
################################################################################

# These targets don't correspond to actual files
.PHONY: run

# Run the simulator with the specified test
run: $(SIM_EXECUTABLE) $(TEST) assemble
	@./$(SIM_EXECUTABLE) $(TEST)

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
	@printf "\t    Assembles the specified $bTEST$n program into hex files\n"
	@printf "\t    for each code section. The hex files are placed in the\n"
	@printf "\t    test's directory under $u<test_name>.<section>.hex$n.\n"
	@printf "\t    A dissabmely of the compiled test is created at\n"
	@printf "\t    $u<test_name>.$(DISAS_EXTENSION)$n.\n"
	@printf "\n"
	@printf "\t$brun$n\n"
	@printf "\t    Invoke the simulator riscv-sim on the specified $bTEST$n program.\n"
	@printf "\t    (Will build the simulator and/or asssemble $bTEST$n program\n"
	@printf "\t     as necessary.)\n"
	@printf "\n"
	@printf "\t$bclean$n\n"
	@printf "\t    Cleans up the files generated by compilation.\n"
	@printf "\n"
	@printf "\t$bveryclean$n\n"
	@printf "\t    Takes the same steps as the $bclean$n target and also\n"
	@printf "\t    cleans up all hex files and disassembly generated from\n"
	@printf "\t    assembling the tests in the project directory.\n"
	@printf "\n"
	@printf "$bVariables:$n\n"
	@printf "\t$bTEST$n\n"
	@printf "\t    The program to assemble. This is a single RISC-V assembly\n"
	@printf "\t    or C file. Defaults to $u$(DEFAULT_TEST)$n.\n"
	@printf "\n"
	@printf "$bExamples:$n\n"
	@printf "\tmake build\n"
	@printf "\tmake assemble TEST=inputs/mytest.S\n"
	@printf "\tmake run TEST=inputs/mytest.S\n"
	@printf "\tmake help\n"
