
CC       := gcc

CFLAGS   := -Wall -Wextra -std=c11 -g -Iinclude -D_POSIX_C_SOURCE=200809L
LDFLAGS  := -Wl,-rpath,'$$ORIGIN/../lib' -Llib

CHECK_CFLAGS := $(shell pkg-config --cflags check 2>/dev/null)
CHECK_LIBS   := $(shell pkg-config --libs   check 2>/dev/null)

BIN_DIR  := bin
OBJ_DIR  := obj

# All C sources 
TEST_SRC := $(wildcard tests/*.c)
CODE_SRC := $(wildcard src/*.c)

# Object files live in obj where we can clean them up
TEST_OBJ := $(TEST_SRC:tests/%.c=$(OBJ_DIR)/%.o)
CODE_OBJ := $(CODE_SRC:src/%.c=$(OBJ_DIR)/%.o)

TARGET   := $(BIN_DIR)/test_graph

.PHONY: all clean test setup-lib

all: $(TARGET)

# Create directories as needed
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# Rule for building object files into obj/
$(OBJ_DIR)/%.o: tests/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CHECK_CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: src/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CHECK_CFLAGS) -c $< -o $@

# Linking step
$(TARGET): $(BIN_DIR) $(OBJ_DIR) $(TEST_OBJ) $(CODE_OBJ)
	$(CC) -o $@ $(TEST_OBJ) $(CODE_OBJ) $(LDFLAGS) -lgraph $(CHECK_LIBS)

test: $(TARGET)
	@echo "==> Running unit tests"
	@./$(TARGET)

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

# Architecture-aware setup of libgraph.so 
setup-lib:
	@arch=$$(dpkg --print-architecture 2>/dev/null || uname -m); \
	if [ "$$arch" = "amd64" ] || [ "$$arch" = "x86_64" ]; then \
		cp lib/libgraph-linux-amd64.so lib/libgraph.so; \
		echo "Using libgraph-linux-amd64.so"; \
	else \
		cp lib/libgraph-linux-arm64.so lib/libgraph.so; \
		echo "Using libgraph-linux-arm64.so"; \
	fi