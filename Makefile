CC = gcc
CFLAGS = -Wall -Iinclude -Ilib/raylib/raylib-5.0_linux_amd64/include -DDEBUG -ggdb
LFLAGS = -L./lib/raylib/raylib-5.0_linux_amd64/lib -lm -lpthread -ldl -l:libraylib.a

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

# List all the source files
SRC_FILES = $(filter-out $(SRC_DIR)/main.c, $(wildcard $(SRC_DIR)/*.c))

# Generate the corresponding object file names
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

# Target: the final executable
TARGET = platformer

# Test source and object files
TEST_SRC = test/test_load_level.c
TEST_OBJ = $(BUILD_DIR)/test_load_level.o
TEST_TARGET = test_load_level

# Default target, build the executable
all: $(BUILD_DIR) $(TARGET)

# Rule to create the build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Rule to build the executable
$(TARGET): $(OBJ_FILES) $(BUILD_DIR)/main.o	
	$(CC) -o $@ $^ $(LFLAGS)

# Rule to build object files from source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<


# =======TEST=======
# Rule to build the test program
$(TEST_TARGET): $(OBJ_FILES) $(TEST_OBJ)
	$(CC) -o $@ $^ $(LFLAGS)

# Rule to build test object file from test source file
$(BUILD_DIR)/test_load_level.o: $(TEST_SRC)
	$(CC) $(CFLAGS) -c -o $@ $<
# =======TEST=======


# Clean rule to remove generated files
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

compile_commands.json: $(SRC_FILES)
	bear -- make

# Phony target to avoid conflicts with file names
.PHONY: all clean test

# =======TEST=======
# Target to build and run tests
test: $(BUILD_DIR) $(TEST_TARGET)

run_test: $(TEST_TARGET)
	./$(TEST_TARGET)
# =======TEST=======
