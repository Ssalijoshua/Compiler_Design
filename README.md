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

### Prerequisites

- `gcc` (GNU C Compiler)
- `make` (Build automation tool)

### Make Commands

#### 1. Build the compiler

```bash
make
```

This compiles all source files and generates the `compiler` executable.

#### 2. Clean build artifacts

```bash
make clean
```

Removes all object files (`.o`) and the executable to prepare for a fresh build.

#### 3. Clean and rebuild

```bash
make clean && make
```

Useful when you've made changes and want a complete fresh compilation.

#### 4. Display help

```bash
make help
```

Shows all available make targets and their descriptions.

### Complete Build Workflow

```bash
# 1. Clone/navigate to project directory
cd Compiler_Design

# 2. Build the compiler
make

# 3. Run lexer on an example file
./compiler examples/test_lexer.c

# 4. Clean up when done
make clean
```

## Using the Lexical Analyzer

The lexical analyzer tokenizes C source code into tokens for further processing.

### Running the Lexer on a File

After building the compiler, you can run the lexer on any C source file:

```bash
./compiler <source_file >
```

This will output a formatted table showing:

- **LEXEME** - The actual token string
- **TOKEN TYPE** - Classification (KEYWORD, IDENTIFIER, LITERAL, OPERATOR, SEPARATOR, EOF, ERROR)
- **LINE** - Line number in the source file
- **COLUMN** - Column position in the source file

### Example: Lexical Analysis

```bash
# Analyze the test file
./compiler examples/test_lexer.c

# Output preview:
# Lexical Analysis of: examples/test_lexer.c

# LEXEME               TOKEN TYPE           LINE     COLUMN
# int                  KEYWORD              2        1
# main                 IDENTIFIER           2        4
# (                    SEPARATOR            2        7
# ...
```

### Supported Tokens

1. **Keywords**: `int`, `float`, `char`, `double`, `void`, `return`, `if`, `else`, `while`, `for`, `do`, `break`, `continue`, `struct`, `typedef`, `switch`, `case`, `default`
2. **Identifiers**: Variable names, function names (alphanumeric + underscore)
3. **Literals**:
   - Integers: `10`, `255`, `0`
   - Floating-point: `3.14`, `2.5`
   - Strings: `"Hello World"`
   - Characters: `'a'`, `'\n'`
4. **Operators**: `+`, `-`, `*`, `/`, `%`, `=`, `==`, `!=`, `<`, `>`, `<=`, `>=`, `&`, `|`
5. **Separators**: `{`, `}`, `(`, `)`, `[`, `]`, `;`, `,`, `.`
6. **Comments**: Single-line (`//`) and block comments (`/* */`)

### Running Examples

We provide example C programs to test the lexer:

```bash
# Run the lexer on the basic test file
./compiler examples/test_lexer.c
```

To create your own test file, simply create a `.c` file in the `examples/` directory and run:

```bash
./compiler examples/your_file.c
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
