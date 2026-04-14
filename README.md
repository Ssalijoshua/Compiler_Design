# C Compiler Project

A compiler for the C programming language that translates C source code to machine code.

## Project Structure

```
Compiler_Design/
├── src/                    # Source files (.c)
│   ├── main.c             # Main compiler driver
│   ├── lexer.c            # Lexical analyzer
│   ├── parser.c           # Parser (syntax analyzer)
│   ├── ast.c              # Abstract syntax tree
│   ├── symtab.c           # Symbol table manager
│   ├── semantic.c         # Semantic analyzer
│   ├── ir.c               # Intermediate code generator
│   ├── codegen.c          # Code generator
│   ├── error.c            # Error handler
│   └── utils.c            # Utility functions
│
├── include/               # Header files (.h)
│   ├── lexer.h
│   ├── parser.h
│   ├── ast.h
│   ├── symtab.h
│   ├── semantic.h
│   ├── ir.h
│   ├── codegen.h
│   ├── error.h
│   └── utils.h
│
├── tests/                 # Unit tests and test cases
├── examples/              # Example C programs to compile
├── output/                # Generated assembly/machine code
├── Makefile               # Build configuration
└── README.md              # This file
```

## Compilation Phases

1. **Lexical Analysis (Lexer)** - Tokenizes input C source code
2. **Syntax Analysis (Parser)** - Builds Abstract Syntax Tree (AST)
3. **Semantic Analysis** - Type checking, symbol resolution, scope verification
4. **Intermediate Code Generation (IR)** - Converts AST to intermediate representation
5. **Code Generation** - Converts IR to target machine code

## Building the Compiler

```bash
make              # Build the compiler
make clean        # Remove build artifacts
make run          # Build and run
make help         # Display help
```

## Modules

### Lexer (lexer.h/lexer.c)

- Tokenizes C source code
- Handles keywords, identifiers, operators, literals
- Manages input stream and position tracking

### Parser (parser.h/parser.c)

- Parses token stream using C grammar rules
- Builds AST from parsed tokens
- Implements recursive descent or other parsing strategy

### AST (ast.h/ast.c)

- Defines all node types for the syntax tree
- Provides tree construction and traversal utilities
- Represents program structure

### Symbol Table (symtab.h/symtab.c)

- Manages variable and function symbols
- Handles scope management
- Performs symbol lookups and insertions

### Semantic Analyzer (semantic.h/semantic.c)

- Performs type checking
- Validates symbol usage and declarations
- Detects semantic errors

### IR Generator (ir.h/ir.c)

- Converts AST to intermediate representation
- Typically uses Three-Address Code (TAC) or similar
- Platform-independent code representation

### Code Generator (codegen.h/codegen.c)

- Converts IR to target machine code
- Handles register allocation
- Generates assembly output

### Error Handler (error.h/error.c)

- Manages error reporting with location info
- Supports error recovery
- Provides clear error messages

### Utilities (utils.h/utils.c)

- Memory management helpers
- String operations
- Data structure implementations
- File I/O utilities
