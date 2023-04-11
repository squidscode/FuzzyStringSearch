GCC=g++
FLAGS=--std=c++11
SRC_DIR=src
# SRC_FILES=$(wildcard $(SRC_DIR)/*.hpp) src/data_structures/FA/DFA.hpp src/data_structures/FA/FA.hpp src/data_structures/FA/NFA.hpp
SRC_FILES:=$(shell find $(SRC_DIR) -name "*.hpp") $(shell find $(SRC_DIR) -name "*.cpp")
OBJ_DIR=obj
OBJ_FILES:=$(patsubst $(SRC_DIR)/%.hpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
TEST_DIR=tests
TEST_FILES:=$(wildcard $(TEST_DIR)/*.cpp)
BIN_DIR=bin
TEST_BIN:=$(subst $(TEST_DIR),$(BIN_DIR),$(patsubst %.cpp, %, $(TEST_FILES)))

all: $(BIN_DEST)

$(TEST_BIN) :
	@find $(SRC_DIR) | grep `echo $@ | sed s#$(BIN_DIR)/##g` | xargs -I %s -- $(GCC) -o $@ $(FLAGS) %s
	@find $(TEST_DIR) | grep `echo $@ | sed s#$(BIN_DIR)/##g` | xargs -I %s -- $(GCC) -o $@ $(FLAGS) %s
	@if [ -f $@ ]; then \
	 	echo "Attempting to create:" $@ " [CREATED]"; \
	 else echo "Attempting to create:" $@ " [ERROR]"; \
	 	fi;


.PHONY: test clean

test: clean $(TEST_BIN)
	@# echo "The source files:" $(SRC_FILES)

clean:
	@echo "Deleting all files in the 'bin' directory."
	-@rm -r $(BIN_DIR)
	-@mkdir $(BIN_DIR)