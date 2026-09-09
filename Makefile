CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Iinclude
BUILD_DIR := build
SOURCES := src/compiler.c src/lexer.c src/parser.c src/symbols.c

.PHONY: all test clean

all: $(BUILD_DIR)/codegen

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/codegen: $(SOURCES) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SOURCES) -o $@

$(BUILD_DIR)/test_runner: $(SOURCES) tests/test_compiler.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -DTEST_COMPILER $(SOURCES) tests/test_compiler.c -o $@

test: $(BUILD_DIR)/test_runner
	./$(BUILD_DIR)/test_runner

clean:
	rm -rf $(BUILD_DIR)
	find tests/fixtures/programs -name '*.vm' -type f -delete
