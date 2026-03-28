
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = 

SRC_DIR = src
BUILD_DIR = build

SRC = $(shell find $(SRC_DIR) -name "*.c")
OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))

BIN = sysasm

ifeq ($(OS),Windows_NT)
    BIN := $(BIN).exe
    MKDIR = mkdir -p
else
    MKDIR = mkdir -p
endif

all: $(BIN)

$(BIN): $(OBJ)
	@$(CC) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@$(MKDIR) $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(BUILD_DIR) $(BIN) $(BIN).exe

run:
	@./$(BIN) --help