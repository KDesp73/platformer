CC = gcc
CFLAGS = -Wall -Iinclude -Ilib/raylib/raylib-5.0_linux_amd64/include
LFLAGS = -L./lib/raylib/raylib-5.0_linux_amd64/lib -lm -lpthread -ldl -l:libraylib.a

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

# List all the source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)

# Generate the corresponding object file names
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

# Target: the final executable
TARGET = platformer

# Default target, build the executable
all: $(BUILD_DIR) $(TARGET)

# Rule to create the build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Rule to build the executable
$(TARGET): $(OBJ_FILES)
	$(CC) -o $@ $^ $(LFLAGS)

# Rule to build object files from source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean rule to remove generated files
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

compile_commands.json: $(SRC_FILES)
	bear -- make

# Phony target to avoid conflicts with file names
.PHONY: all clean

