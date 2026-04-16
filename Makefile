# Makefile for C Compiler Project

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I./include
LDFLAGS = 
SOURCES = src/main.c src/lexer.c src/parser.c src/ast.c src/semantic.c src/symtab.c src/ir.c src/codegen.c src/error.c src/utils.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = compiler

# Default target
all: $(EXECUTABLE)

# Build executable
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

# Run the compiler
run: $(EXECUTABLE)
	./$(EXECUTABLE)

# Help target
help:
	@echo "Available targets:"
	@echo "  make          - Build the compiler"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make run      - Build and run the compiler"
	@echo "  make help     - Display this help message"

.PHONY: all clean run help
