CXXFLAGS = -O3 -std=c++17 # -Wall -Wextra
LIBS = -I /usr/local/include/boost/
SRC_DIR = ./
OUTPUT_DIR = ../bin/
TESTCASE_DIR = ../testcase/
OUTPUT_DIR_PUBLIC = ../output/
VERIFY_DIR = ../verifier/
PROGRAM_NAME = hw2

# Automatically find all .cpp files in the src directory
SRC = $(wildcard $(SRC_DIR)*.cpp)

# Object files corresponding to the source files
OBJ = $(SRC:$(SRC_DIR)%.cpp=$(OUTPUT_DIR)%.o)

# Executable target
EXEC = $(OUTPUT_DIR)$(PROGRAM_NAME)

# Parallel compilation
MAKEFLAGS := -j

ifneq ($(filter run verify run_verify,$(firstword $(MAKECMDGOALS))),)
  TESTCASE := $(word 2, $(MAKECMDGOALS))
  ifneq ($(TESTCASE),)
    $(eval $(TESTCASE):;@:)
  endif
endif


# Rule to build the executable
$(EXEC): $(OBJ)
	@mkdir -p $(OUTPUT_DIR)
	@g++ $(CXXFLAGS) $(OBJ) -o $(EXEC)

# Rule to build object files
$(OUTPUT_DIR)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(OUTPUT_DIR)
	@g++ $(CXXFLAGS) -c $< -o $@

all: $(EXEC)

clean:
	@rm -f $(OBJ) $(EXEC)

run: $(EXEC)
	$(EXEC) $(TESTCASE_DIR)public$(TESTCASE).txt $(OUTPUT_DIR_PUBLIC)public$(TESTCASE).out

verify:
	$(VERIFY_DIR)verify $(TESTCASE_DIR)public$(TESTCASE).txt $(OUTPUT_DIR_PUBLIC)public$(TESTCASE).out

run_verify: $(EXEC)
	$(EXEC) $(TESTCASE_DIR)public$(TESTCASE).txt $(OUTPUT_DIR_PUBLIC)public$(TESTCASE).out
	$(VERIFY_DIR)verify $(TESTCASE_DIR)public$(TESTCASE).txt $(OUTPUT_DIR_PUBLIC)public$(TESTCASE).out

sample: $(EXEC)
	$(EXEC) $(TESTCASE_DIR)sample.txt $(OUTPUT_DIR_PUBLIC)sample.out
	$(VERIFY_DIR)verify $(TESTCASE_DIR)sample.txt $(OUTPUT_DIR_PUBLIC)sample.out

.PHONY: all clean run verify sample