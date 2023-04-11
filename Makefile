GCC=g++
FLAGS=--std=c++11
SRC_DIR=src
SRC_FILES=$(wildcard $(SRC_DIR)/*.hpp) src/data_structures/FA/DFA.hpp src/data_structures/FA/FA.hpp src/data_structures/FA/NFA.hpp
OBJ_DIR=obj
OBJ_FILES:=$(patsubst $(SRC_DIR)/%.hpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
TEST_DIR=tests
TEST_FILES:=$(wildcard $(TEST_DIR)/*.cpp)
BIN_DIR=bin
TEST_BIN:=$(subst $(TEST_DIR),$(BIN_DIR),$(patsubst %.cpp, %, $(TEST_FILES)))

all: $(BIN_DEST)

$(OBJ_FILES) :
	mkdir -p `echo $@ | sed -E 's#/[a-zA-Z]*\.[a-zA-Z]*$$##g'`
	$(GCC) $(FLAGS) -c -o $@ `echo $@ | sed -E 's#$(OBJ_DIR)#$(SRC_DIR)#g' | sed -E 's@.o@.hpp@g'`

$(TEST_BIN) : $(OBJ_FILES)
	echo $(SRC_FILES)
	find $(SRC_DIR) | grep `echo $@ | sed s#$(BIN_DIR)/##g` | xargs -I %s -- $(GCC) -o $@ $(FLAGS) $(OBJ_FILES) %s
	find $(TEST_DIR) | grep `echo $@ | sed s#$(BIN_DIR)/##g` | xargs -I %s -- $(GCC) -o $@ $(FLAGS) $(OBJ_FILES) %s

.PHONY: test clean

test: clean $(TEST_BIN)
	echo $(TEST_BIN)

clean:
	rm -r $(BIN_DIR)
	mkdir $(BIN_DIR)