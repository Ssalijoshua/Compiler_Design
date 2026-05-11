# Compiler Design Components - Presentation Questions & Answers

**Document Purpose:** Comprehensive reference for compiler implementation with specific file references, line numbers, and code examples for each compiler design concept.

**Total Content:** 2,187 lines covering 14 major topics with 46+ detailed questions

**Quick Navigation:**

1. [Lexical Analysis](#1-lexical-analysis-lexer) - Tokenization (3 questions)
2. [Syntax Analysis](#2-syntax-analysis-parser) - Parsing with TOP-DOWN RECURSIVE DESCENT (3 questions)
3. [Abstract Syntax Tree](#3-abstract-syntax-tree-ast) - Tree representation (3 questions)
4. [Semantic Analysis](#4-semantic-analysis) - Type checking & symbols (3 questions)
5. [Symbol Table](#5-symbol-table) - Scope & declarations (3 questions)
6. [IR Generation](#6-intermediate-code-generation-ir) - Three-Address Code (4 questions)
7. [Code Generation](#7-code-generation) - x86-64 Assembly (3 questions)
8. [Error Handling](#8-error-handling) - Diagnostics & recovery (3 questions)
9. [Utility Functions](#9-utility-functions) - Memory & string support (3 questions)
10. [Phase Integration](#10-compiler-phases---integration-questions) - Component interaction (4 questions)
11. [Testing Modes](#11-compiler-modes-for-testing) - Incremental debugging (2 questions)
12. [Application](#12-practical-application-scenarios) - Real usage examples (2 questions)
13. [Improvements](#13-challenges-and-improvements) - Limitations & roadmap (2 questions)
14. [Discussion](#14-presentation-discussion-points) - Architecture & comparison (2 questions)

**Key Facts:**

- **Parsing Technique:** TOP-DOWN RECURSIVE DESCENT (not bottom-up)
- **Implementation:** ~3,120 lines of C code (10 source + 10 header files)
- **Compilation Phases:** 5 phases (Lexer → Parser → Semantic → IR → Codegen)
- **Assembly Target:** x86-64 with 10+ instruction types
- **Testing:** 5 compiler modes for phase-by-phase validation

---

## 1. LEXICAL ANALYSIS (LEXER)

**Files:**

- Header: `include/lexer.h`
- Implementation: `src/lexer.c`
- Main Entry: `Token get_next_token(FILE *fp)` in src/lexer.c (line ~25)

### Question 1.1

**Where is the Lexer applied in the compiler pipeline?**

**Answer:**
The Lexer is the **first phase** of compilation. It:

- Reads the source code character-by-character
- Converts character sequences into tokens (keywords, identifiers, literals, operators, separators)
- Tracks line and column numbers for error reporting
- Filters out comments and whitespace
- Example: Converts `int x = 10;` into tokens: `[int][x][=][10][;]`

**Implementation Details:**

- Static variables `line` and `column` track position (src/lexer.c, line 6-7)
- Function `is_keyword()` checks against 17 reserved keywords (src/lexer.c, line 9)
- Handles escape sequences in strings/characters (src/lexer.c, line 197-200)
- Token structure: `TokenType type, char lexeme[256], int line, int column` (include/lexer.h, line 19)

### Question 1.2

**What types of tokens does the Lexer recognize?**

**Answer:**
The Lexer recognizes 7 token types (defined in `include/lexer.h`, line 6-14):

1. **TOKEN_KEYWORD** - Reserved words (int, return, if, while, for, etc.)
2. **TOKEN_IDENTIFIER** - Variable/function names (x, count, main, etc.)
3. **TOKEN_LITERAL** - Constants (10, 3.14, "hello", 'a')
4. **TOKEN_OPERATOR** - Arithmetic/logical operators (+, -, \*, /, %, ==, !=, <, >, etc.)
5. **TOKEN_SEPARATOR** - Delimiters ({, }, (, ), ;, ,, [, ])
6. **TOKEN_EOF** - End of file marker
7. **TOKEN_ERROR** - Invalid characters

**Keyword List Implementation:**

- Keywords array defined in `src/lexer.c` (line 10-16)
- 17 keywords supported: int, return, if, else, void, char, float, double, while, for, do, break, continue, struct, typedef, switch, case, default
- Checked via `is_keyword()` function (src/lexer.c, line 9)

### Question 1.3

**How does the Lexer handle comments and multi-character operators?**

**Answer:**

- **Single-line comments** (`//`): Skips all characters until newline (src/lexer.c, line 40-50)
- **Block comments** (`/* ... */`): Skips all characters between markers (src/lexer.c, line 51-69)
- **Multi-character operators**: Recognized as single tokens (e.g., `==`, `!=`, `<=`, `>=`)
- The Lexer maintains state to properly count lines and columns during comment skipping

**Implementation Details:**

- Comments are handled in main loop of `get_next_token()` (src/lexer.c, line 35)
- Line and column tracking: increment line on `'\n'`, reset column to 1 (src/lexer.c, line 37-43)
- Lookahead character `next = fgetc(fp)` used to detect comment start (src/lexer.c, line 39)
- `ungetc()` used to return lookahead character if not comment (src/lexer.c, line 67)

---

## 2. SYNTAX ANALYSIS (PARSER)

**Files:**

- Header: `include/parser.h`
- Implementation: `src/parser.c` (1154 lines)
- Techniques: **TOP-DOWN RECURSIVE DESCENT PARSING**
- Main Entry: `ASTNode *parser_parse(Parser *parser)` in src/parser.c (line ~110)

### Question 2.1

**Where is the Parser applied and what does it do?**

**Answer:**
The Parser is the **second phase** of compilation. It:

- Takes the token stream from the Lexer
- Builds an **Abstract Syntax Tree (AST)** based on grammar rules
- Implements **TOP-DOWN recursive descent parsing** with predictive lookahead
- Validates the syntactic structure of the program
- Reports syntax errors with line/column information
- Example: Parses `int x = 10;` into a VAR_DECL node with initializer

**Parser Structure:**

- Parser state structure: `typedef struct { FILE *input_file, Token current_token, error_count, warning_count }` (include/parser.h, line 9-14)
- Key functions: `parser_create()`, `parser_free()`, `parser_parse()` (include/parser.h, line 17-19)
- Error handling: `parser_error()` and `parser_warning()` (include/parser.h, line 22-23)

### Question 2.2

**What is recursive descent parsing and how is it implemented?**

**Parsing Technique: TOP-DOWN RECURSIVE DESCENT**

Recursive descent parsing in this compiler:

- Uses recursive functions for each grammar production
- Each function matches one grammar rule/non-terminal
- **Direction**: Starts from top-level `parse_program()` and descends to primitives
- **Lookahead**: Uses `current_token` with predictive lookahead to decide which rule to apply
- No backtracking needed due to grammar design

**Implementation Functions (src/parser.c):**

- `parse_declaration()` (line ~160) - Top-level declarations
- `parse_variable_declaration()` (line ~190) - Variable declarations
- `parse_statement()` (line ~240) - Statements
- `parse_block_statement()` (line ~260) - Compound statements {...}
- `parse_if_statement()` (line ~290) - if/else statements
- `parse_while_statement()` (line ~320) - while loops
- `parse_for_statement()` (line ~350) - for loops
- `parse_return_statement()` (line ~400) - return statements
- `parse_expression_statement()` (line ~430) - Expression statements
- `parse_expression()` → `parse_assignment_expression()` → ... → `parse_primary_expression()` (line ~470-750)

**Function Hierarchy:**

```
parse_program() [entry point]
  ├─ parse_declaration()
  │   ├─ parse_variable_declaration()
  │   └─ parse_function_decl() [builds FUNCTION_DECL node]
  │
  └─ parse_statement()
      ├─ parse_block_statement()
      ├─ parse_if_statement()
      ├─ parse_while_statement()
      ├─ parse_for_statement()
      ├─ parse_return_statement()
      └─ parse_expression_statement()
          └─ parse_expression()
              └─ parse_assignment_expression() [precedence climbing]
                  └─ parse_logical_or_expression()
                      └─ parse_logical_and_expression()
                          └─ parse_equality_expression()
                              └─ parse_relational_expression()
                                  └─ parse_additive_expression()
                                      └─ parse_multiplicative_expression()
                                          └─ parse_unary_expression()
                                              └─ parse_postfix_expression()
                                                  └─ parse_primary_expression() [base case]
```

**Helper Functions (src/parser.c):**

- `advance_token()` (line ~780) - Move to next token
- `check_token()` (line ~790) - Verify token type
- `synchronize()` (line ~795) - Error recovery

### Question 2.3

**What grammar constructs does the Parser handle?**

**Answer:**
The Parser handles (implementation in src/parser.c):

1. **Declarations** - Functions and variables
   - Parse functions: `parse_declaration()` (line ~160), `parse_variable_declaration()` (line ~190)
   - Supports variable initializers and function parameters
2. **Statements** - Blocks, if/else, while, for, return, break, continue
   - Parse functions: `parse_block_statement()` (line ~260), `parse_if_statement()` (line ~290), `parse_while_statement()` (line ~320), `parse_for_statement()` (line ~350)
3. **Expressions** - Binary/unary operations, assignments, function calls, array access, literals
   - Parse functions: `parse_expression()` (line ~470) through `parse_primary_expression()` (line ~740)
4. **Operator precedence** - Handles proper precedence for arithmetic and logical operators
   - Implemented via cascading function calls (additive before multiplicative, etc.)
   - Precedence hierarchy ensures: `* / % ` > `+ -` > `< <= > >=` > `==` > `!=` > `&&` > `||`
5. **Error recovery** - Uses synchronization to recover from parse errors
   - `synchronize()` function (src/parser.c, line ~795) skips to next top-level keyword on error

---

## 3. ABSTRACT SYNTAX TREE (AST)

**Files:**

- Header: `include/ast.h` (234 lines)
- Implementation: `src/ast.c` (634 lines)
- Structures: 24 node types with union-based data (include/ast.h, line 14-46)

### Question 3.1

**What is the AST and how is it structured in this compiler?**

**Answer:**
The AST is a **tree representation of program structure** (defined in include/ast.h):

- **Provides abstract view** - ignores irrelevant syntax (comments, semicolons)
- **Node types** (24 types, enum NodeType at line 13-46):
  - Top-level: PROGRAM, FUNCTION_DECL, VAR_DECL
  - Statements: BLOCK, IF_STMT, WHILE_STMT, FOR_STMT, RETURN_STMT, BREAK_STMT, CONTINUE_STMT, EXPRESSION_STMT
  - Expressions: BINARY_OP, UNARY_OP, ASSIGNMENT, FUNCTION_CALL, ARRAY_ACCESS, IDENTIFIER, LITERAL, CAST
- **Union-based data structure** (include/ast.h, line 55-188):
  - Generic ASTNode structure with union containing type-specific fields
  - Each node type has specific fields accessible via union
  - Example: NODE_BINARY_OP has operator, left, right fields
- **Supports debugging**: Can be printed in tree format via `ast_print()` (src/ast.c, line ~460)

**Node Structure (include/ast.h, line 49-194):**

```c
typedef struct ASTNode {
    NodeType type;
    int line;
    int column;
    union {
        struct { /* NODE_PROGRAM */ } program;
        struct { /* NODE_FUNCTION_DECL */ } function_decl;
        struct { /* NODE_VAR_DECL */ } var_decl;
        // ... 21 more node types
    } data;
}
```

### Question 3.2

**How does the AST represent different program constructs?**

**Answer:**
Examples of AST representation (nodes created in src/ast.c):

- **Variable declaration**: `int x = 10;`
  ```
  VAR_DECL (name=x, type=int)          [ast_create_var_decl(), src/ast.c:75]
    └─ LITERAL (value=10)               [ast_create_literal_int(), src/ast.c:263]
  ```
- **If statement**: `if (x > 5) { return x; }`
  ```
  IF_STMT                               [ast_create_if_stmt(), src/ast.c:119]
    ├─ Condition: BINARY_OP (>)         [ast_create_binary_op(), src/ast.c:201]
    │   ├─ IDENTIFIER (x)               [ast_create_identifier(), src/ast.c:251]
    │   └─ LITERAL (5)                  [ast_create_literal_int(), src/ast.c:263]
    └─ Then: BLOCK                      [ast_create_block(), src/ast.c:105]
        └─ RETURN_STMT                  [ast_create_return_stmt(), src/ast.c:141]
            └─ IDENTIFIER (x)           [ast_create_identifier(), src/ast.c:251]
  ```

**Creation Functions (src/ast.c, 43 total):**

- `ast_create_node()` (line 14) - Generic node creation
- `ast_create_program()` (line 26) - Program node
- `ast_create_function_decl()` (line 35) - Function declaration
- `ast_create_var_decl()` (line 53) - Variable declaration
- `ast_create_binary_op()` (line 189) - Binary operation
- `ast_create_if_stmt()` (line 113) - If statement
- ... 37 more creation functions

### Question 3.3

**Why is the AST important for subsequent compiler phases?**

**Answer:**
The AST is crucial because:

1. **Semantic Analysis** uses it to check types and symbols
   - `semantic_analyze()` walks AST (src/semantic.c, line 26)
2. **IR Generation** traverses it to create intermediate code
   - `ir_generate()` recursively processes AST nodes (src/ir.c, line 192)
3. **Code Generation** walks it to generate assembly
   - `codegen_generate()` iterates IR instructions (src/codegen.c, line 76)
4. **Error Recovery** is easier on AST than token stream
   - Parse errors reference AST line/column info
5. **Structure clarity** - easy to understand and manipulate the program
   - `ast_print()` displays tree structure for debugging (src/ast.c, line ~460)
   - `ast_free()` recursively deallocates tree (src/ast.c, line ~320)

---

## 4. SEMANTIC ANALYSIS

**Files:**

- Header: `include/semantic.h`
- Implementation: `src/semantic.c` (253 lines)
- Main Entry: `int semantic_analyze(SemanticAnalyzer *analyzer, ASTNode *ast)` (src/semantic.c, line 26)

### Question 4.1

**What does the Semantic Analyzer do and where is it applied?**

**Answer:**
The Semantic Analyzer is the **third phase** (src/main.c, line ~130-150). It:

- Validates the **meaning** of the program
- Performs **type checking** (ensuring operations are valid for types)
- **Resolves symbols** (links identifiers to their declarations) via symbol table
- **Manages scopes** (global, function, block scopes) using `symtab_enter_scope()/exit_scope()`
- Detects semantic errors (undefined variables, redefined symbols)
- Works on the AST produced by the Parser
- Example: Verifies `x + y` where both x and y are declared as int

**Analyzer Structure:**

- `typedef struct { SymbolTable *symtab, int error_count }` (include/semantic.h, line 12-15)
- Main functions: `semantic_create()`, `semantic_analyze()`, `semantic_free()` (include/semantic.h, line 17-19)

### Question 4.2

**How does the Semantic Analyzer handle scopes?**

**Answer:**
Scope management implementation (src/semantic.c, `semantic_check_node()` line 39):

- **Scope levels**: Tracks nesting depth (0=global, 1+=function/block scopes)
  - Stored in `SymbolTable.scope_level` (include/symtab.h, line 32)
- **Enter/Exit**: `symtab_enter_scope()` on block/function entry (src/symtab.c, line 31), `symtab_exit_scope()` on exit (src/symtab.c, line 40)
  - Called in: NODE_PROGRAM (line 51), NODE_FUNCTION_DECL (line 57-59), NODE_BLOCK (line 95-97), NODE_FOR_STMT (line 126-128)
- **Scope chain lookup**: Variables searched from current scope up to global
  - `symtab_lookup()` searches backwards through symbol array (src/symtab.c, line 108)
  - Only returns symbols accessible from current scope
- **Shadowing**: Inner scopes can redefine outer scope variables
  - Allowed if symbol not in current scope (src/symtab.c, line 58-62)
- **Cleanup**: Symbols are removed when exiting scope
  - `symtab_exit_scope()` removes symbols from exited scope (src/symtab.c, line 40-48)
- Example: In function body, a variable declared in function scope shadows global variable

**Scope Example Code (src/semantic.c):**

```c
case NODE_BLOCK:
    symtab_enter_scope(analyzer->symtab);  // line 95
    for (int i = 0; i < node->data.block.num_statements; i++) {
        semantic_check_node(analyzer, node->data.block.statements[i]);
    }
    symtab_exit_scope(analyzer->symtab);   // line 99
    break;
```

### Question 4.3

**What semantic errors does the analyzer detect?**

**Answer:**
Detected errors (src/semantic.c, error reporting at lines 65-70, 84-89, 134-139, 149-152):

1. **Undefined Symbols** - Using variables/functions not declared
   - Detected: `symtab_lookup()` returns NULL (src/semantic.c, line 150)
   - Error code: `ERROR_UNDEFINED_SYMBOL` (src/err.c, ~line 25)
2. **Redefined Symbols** - Declaring same variable twice in same scope
   - Detected: `symtab_define()` return 0 on conflict (src/semantic.c, line 65)
   - Error code: `ERROR_REDEFINED_SYMBOL` (src/error.c, ~line 25)
3. **Type Mismatches** - Operations on incompatible types (future enhancement)
   - Framework: `semantic_infer_type()` (src/semantic.c, line 233)
4. **Undefined Functions** - Calling non-existent functions
   - Detected: Node type NODE_FUNCTION_CALL (src/semantic.c, line 135)
5. **Invalid Operations** - Operations that don't make sense for types
   - Detected: Further type checking in `semantic_infer_type()`

**Error Reporting (src/error.c, line 13):**

```c
void error_report(ErrorType type, int line, int column, const char *message)
```

Global counters: `error_count`, `warning_count` (src/error.c, line 11-12)

---

## 5. SYMBOL TABLE

**Files:**

- Header: `include/symtab.h`
- Implementation: `src/symtab.c` (105 lines)
- Data Structure: Flat array `Symbol symbols[MAX_SYMBOLS]` where MAX_SYMBOLS=1000 (include/symtab.h, line 13, 26)

### Question 5.1

**What is the Symbol Table and how is it used?**

**Answer:**
The Symbol Table is a **data structure tracking all declared symbols** (include/symtab.h, line 20-30):

- **Stores**: Symbol name, kind (var/function/parameter), type, scope level, initialization status
  - `typedef struct { char name[256], SymbolKind kind, DataType type, int is_array, int scope_level }` (include/symtab.h, line 20-26)
- **Capacity**: Up to 1000 symbols (MAX_SYMBOLS=1000, include/symtab.h, line 13)
- **Operations**:
  - `symtab_define()` - Add new symbol (src/symtab.c, line 50) - returns 1 on success, 0 if redefined
  - `symtab_lookup()` - Search in current + outer scopes (src/symtab.c, line 82) - backwards search
  - `symtab_lookup_local()` - Search only current scope (src/symtab.c, line 95)
- **Used by**: Semantic analysis (src/semantic.c, line 50+), code generation (src/codegen.c)
- **Example entry**: `(name="x", kind=SYMBOL_VAR, type=TYPE_INT, scope_level=1, is_array=0)`

**Symbol Table Stack Simulation:**
The compiler maintains ONE global symbol table that simulates scope stack:

- All symbols added/kept in single array with scope_level field
- Symbols with higher scope_level are "on stack", lower are "on stack"

### Question 5.2

**How does the Symbol Table handle multiple scopes?**

**Answer:**
Multi-scope handling (src/symtab.c, 13-105):

- **Flat array structure**: Single array with scope_level field
  - `Symbol symbols[1000]` where each symbol has `int scope_level` field (include/symtab.h, line 25)
- **Scope tracking**: Each symbol records its scope_level
  - Assigned in `symtab_define()` (src/symtab.c, line 77): `sym->scope_level = table->scope_level`
- **On scope exit**: Symbols from exited scope are removed from table
  - `symtab_exit_scope()` (src/symtab.c, line 40) removes symbols with higher scope_level
- **Lookup order**: Searches from most recent (highest scope level) to global (level 0)
  - `symtab_lookup()` iterates backwards in array (src/symtab.c, line 82-88)
  - `symtab_lookup_local()` only searches current scope level (src/symtab.c, line 95-103)
- **Prevents conflicts**: Same name allowed in different scopes
  - Check: `if (table->symbols[i].scope_level == table->scope_level)` (src/symtab.c, line 59)
- Example:
  ```
  Global scope (0):     x, y
  ├─ Function scope (1): i, j
  └─ Block scope (2):   x (shadows global)
  ```

**Scope Management Functions:**

- `symtab_enter_scope()` (src/symtab.c, line 31) - increments scope_level
- `symtab_exit_scope()` (src/symtab.c, line 40) - decrements scope_level, removes symbols

### Question 5.3

**Where is the Symbol Table applied throughout compilation?**

**Answer:**
Applied in:

1. **Semantic Analysis** - Define symbols during AST walk, lookup for validation
   - `symtab_define()` called for VAR_DECL, FUNCTION_DECL, PARAM nodes (src/semantic.c, line ~65-90)
   - `symtab_lookup()` called for NODE_IDENTIFIER, NODE_FUNCTION_CALL (src/semantic.c, line ~135-150)

2. **Type Inference** - Retrieve symbol types for type checking
   - `symtab_lookup()` returns Symbol with type field (src/semantic.c, line 244)
   - Used in `semantic_infer_type()` (src/semantic.c, line 235-293)

3. **Code Generation** - Look up variable types to generate correct instructions
   - Could be extended to allocate stack memory based on variable types

4. **Error Reporting** - Get symbol info for error messages
   - Error context from symbol lookup results
5. **Scope Verification**: Ensure variable access is valid in current scope

---

## 6. INTERMEDIATE CODE GENERATION (IR)

**Files:**

- Header: `include/ir.h`
- Implementation: `src/ir.c` (382 lines)
- Data Structure: Array `IRInstruction instructions[MAX_IR_INSTRUCTIONS]` where MAX=10000 (include/ir.h, line 49)
- Main Entry: `int ir_generate(IRGenerator *ir, ASTNode *ast)` (src/ir.c, line 192)

### Question 6.1

**What is Intermediate Representation (IR) and why is it used?**

**Answer:**
IR (Three-Address Code) is a **bridge between high-level AST and low-level assembly** (include/ir.h, line 1-12):

- **Three-Address Code (TAC)**: Each instruction has max 3 operands
  - Structure: `typedef struct { IROpType op, IROperand op1, op2, result }` (include/ir.h, line 41-45)
- **Format**: `result = operand1 op operand2`
- **Benefits**:
  - Independent from target architecture
  - Easier to optimize (no IR optimization pass in current impl)
  - Easier to generate from AST (recursive tree walk)
  - Easier to translate to assembly (instruction selection)
- **Example**: `t0 = a + b` (add a and b, store in temp t0)

**IR Data Structure (include/ir.h, line 30-47):**

```c
typedef enum { IR_ASSIGN, IR_ADD, IR_SUB, ... } IROpType;  // line 33-48
typedef struct { char name[256]; } IROperand;               // line 50-52
typedef struct { IROpType op; IROperand op1, op2, result; } IRInstruction;  // line 54-58
typedef struct {
    IRInstruction instructions[10000];
    int num_instructions;
    int next_label;
    int next_temp;
} IRGenerator;
```

### Question 6.2

**What types of IR instructions does this compiler generate?**

**Answer:**
IR instruction types (19 types, enum IROpType in include/ir.h, line 24-48):

1. **Arithmetic** (src/ir.c, line 298-314):
   - IR_ADD, IR_SUB, IR_MUL, IR_DIV, IR_MOD
   - Emitted by: `ir_emit(ir, IR_ADD, left_temp, right_temp, result)` (src/ir.c, line ~311)

2. **Comparison** (src/ir.c, line 315-326):
   - IR_EQ, IR_NE, IR_LT, IR_LE, IR_GT, IR_GE
   - Emitted by: `ir_emit(ir, IR_EQ, left_temp, right_temp, result)` (src/ir.c, line ~318)

3. **Control Flow** (src/ir.c, line 327-344):
   - IR_LABEL, IR_JUMP, IR_JUMP_IF_ZERO, IR_JUMP_IF_NOT_ZERO
   - Emitted by: `ir_emit_label(ir, label_name)`, `ir_emit_jump(ir, label)`, `ir_emit_jump_if_zero(ir, cond, label)` (src/ir.c, line 68-75)

4. **Function**: IR_CALL, IR_PARAM, IR_RETURN
5. **Assignment**: IR_ASSIGN

- All use standardized format: `result = op1 operator op2`
- Operands can be: variables, literals, temporary variables (t0, t1, ...), labels (L0, L1, ...)

**Emission Function (src/ir.c, line 45):**

```c
void ir_emit(IRGenerator *ir, IROpType op, const char *op1, const char *op2, const char *result)
```

### Question 6.3

**How does IR Generation traverse the AST?**

**Answer:**
Recursive IR generation (src/ir.c, line 233-382):

- **Visit each AST node**: Depth-first traversal via `ir_generate_node()` (src/ir.c, line 233)
- **Generate code for children**: Process operands first (post-order traversal)
  - For BINARY_OP: generate left operand → right operand → combine (src/ir.c, line 313-355)
- **Emit instructions**: For each node type using `ir_emit()` (src/ir.c, line 45)
- **Temporary variables**: Generate unique temps for intermediate values
  - Function `ir_new_temp()` (src/ir.c, line 87) - generates t0, t1, t2...
- **Labels**: Generate unique labels for control flow
  - Function `ir_new_label()` (src/ir.c, line 99) - generates L0, L1, L2...
- Example: For `y = x + 5`
  ```
  t0 = x        (load x into t0, ir_emit(IR_ASSIGN, "x", NULL, "t0"))
  t1 = 5        (load 5 into t1, ir_emit(IR_ASSIGN, "5", NULL, "t1"))
  t2 = t0 + t1  (add, ir_emit(IR_ADD, "t0", "t1", "t2"))
  y = t2        (assign, ir_emit(IR_ASSIGN, "t2", NULL, "y"))
  ```

**Node Processing (src/ir.c, line 233-382):**

```c
static void ir_generate_node(IRGenerator *ir, ASTNode *node, char *result)
```

- Each node type has specific handling (switch statement, line 248-382)
- POST-ORDER traversal ensures children processed before parent

### Question 6.4

**Where is the IR used in the compilation pipeline?**

**Answer:**
IR is used (compilation phases in src/main.c):

1. **After Semantic Analysis**: IR generator takes validated AST
   - Called: `ir_generate(ir, ast)` (src/main.c, line ~160)

2. **Before Code Generation**: Code gen translates IR to assembly
   - Called: `codegen_generate(codegen, ir)` (src/main.c, line ~190)
   - IR acts as interface between AST and assembly

3. **Optimization**: Could optimize IR before code generation
   - Framework exists but no passes implemented currently
   - Would do: constant folding, dead code elimination, loop optimization

4. **Debugging**: IR can be printed to understand program flow
   - Called: `ir_print(ir)` (src/ir.c, line 108)
   - Displays all instructions in readable format
   - Mode `-ir` shows this output (src/main.c, line 157-165)

5. **Portability**: Same IR can target different architectures
   - Current: x86-64 assembly via `codegen_generate()`
   - Could add: ARM, MIPS, RISC-V code generators using same IR

---

## 7. CODE GENERATION

**Files:**

- Header: `include/codegen.h`
- Implementation: `src/codegen.c` (184 lines)
- Main Entry: `int codegen_generate(CodeGenerator *gen, IRGenerator *ir)` (src/codegen.c, line 76)
- Target Architecture: **x86-64 assembly (AT&T syntax)**

### Question 7.1

**What does the Code Generator do and what output does it produce?**

**Answer:**
The Code Generator is the **final compilation phase** (src/main.c, line ~177-200). It:

- **Translates IR to assembly code**: Converts TAC (Three-Address Code) to x86-64 assembly instructions
- **Generates output file**: Creates `.s` assembly file with same base name as source (src/main.c, line ~175)
- **Produces**: Assembly-like instructions with operands in AT&T syntax
- **Handles**:
  - Register allocation (simplified - uses specific registers: rax, al, rdx)
  - Instruction selection (choose appropriate x86-64 instruction for IR operation)
  - Function prologue/epilogue (function labels)
- **Example**: IR `t0 = a + b` becomes (src/codegen.c, line ~166-171):
  ```asm
  mov %a, %rax
  add %b, %rax
  mov %rax, %t0
  ```

**Codegen Structure (include/codegen.h, line 11-15):**

```c
typedef struct {
    FILE *output;              // Output file handle
    int label_count;           // Label generation counter
} CodeGenerator;
```

### Question 7.2

**How does the Code Generator handle different types of operations (Instruction Selection)?**

**Answer:**
Operation handling via **instruction selection mapping** in `codegen_generate_instruction()` (src/codegen.c, line 90):

The code generator processes each IR instruction and emits corresponding x86-64 operations:

**1. Arithmetic Operations (src/codegen.c, line 103-126):**

- **ADD**: `mov operand1 → rax`, then `add operand2, %rax`
- **SUB**: `mov operand1 → rax`, then `sub operand2, %rax`
- **MUL**: `mov operand1 → rax`, then `imul operand2, %rax`
- **DIV**: Load dividend into rax, sign extend with `cqo`, then `idiv divisor` (src/codegen.c, line 115-120)
- **MOD**: Similar to DIV - result in rdx instead of rax (src/codegen.c, line 121-126)

**2. Comparisons (src/codegen.c, line 130-140):**

- `cmp operand2, %rax` (compare rax against operand2)
- `setcc %al` (set byte in al based on condition flags: sete, setne, setl, etc.)
- `movzx %al, result_reg` (zero-extend to full word - converts 0/1 to proper width)

**3. Control Flow (src/codegen.c, line 150-160):**

- **Labels**: `L1:` (plain label)
- **Unconditional jumps**: `jmp L1`
- **Conditional jumps**: `je L1` (jump if equal), `jne L1` (jump if not equal), `jl L1` (jump if less)

**4. Assignment (src/codegen.c, line ~95-100):**

- `mov source, destination` (direct move between registers or memory)

**5. Return (src/codegen.c, line ~155-158):**

- Place return value in rax: `mov return_value, %rax`
- Emit: `ret` instruction

**Register Convention Used (simplified for this compiler):**

- **rax (al for byte)**: Arithmetic operations, return values, division quotient
- **rdx**: Division remainder (modulo results)
- **Variables/temps**: Treated as memory locations or registers as needed

### Question 7.3

**What assembly instructions are generated and why? (Complete x86-64 Instruction Set)**

**Answer:**
Complete instruction set generated (src/codegen.c, line 90-165):

**Data Movement Instructions (src/codegen.c, ~95-102):**

- **mov %reg, %reg** / **mov $imm, %reg** / **mov mem, %reg**: Move values (fprintf() calls emit these)
- **movzx %al, %result**: Move with zero extension
  - Used to convert condition flag result (0 or 1 in al) to full integer width
  - Example: `setne al` followed by `movzx %al, %rax` for comparison results

**Arithmetic Instructions (src/codegen.c, line 103-126):**

- **add %operand2, %rax**: Addition
  - Pattern: First move op1 to rax, then add op2
  - Implements: `result = operand1 + operand2` in IR
- **sub %operand2, %rax**: Subtraction
  - Pattern: First move op1 to rax, then subtract op2
  - Implements: `result = operand1 - operand2` in IR
- **imul %operand2, %rax**: Signed multiplication
  - Result in rax (16-bit multiply may produce 32-bit result, stored in rax)
  - Implements: `result = operand1 * operand2` in IR
- **cqo**: Sign-extend rax to rdx:rax
  - Setup instruction for division: converts 64-bit rax to 128-bit rdx:rax
  - Prepared rax before idiv (src/codegen.c, line ~118)
- **idiv %divisor**: Signed division
  - Dividend in rdx:rax (after cqo), divisor in operand
  - Quotient → rax, remainder → rdx
  - Implements: IR_DIV (quotient) and IR_MOD (remainder)

**Comparison Instructions (src/codegen.c, line 130-140):**

- **cmp %operand2, %rax**: Compare (subtract and discard, set flags)
  - Sets condition flags (Zero, Sign, etc.) based on rax - operand2
  - Used before conditional set/jump instructions
  - Implements: `result = operand1 relop operand2` for comparison IR nodes

**Conditional Set Instructions (src/codegen.c, line ~133-138):**

- **sete %al**: Set if Equal (ZF=1)
- **setne %al**: Set if Not Equal (ZF=0)
- **setl %al**: Set if Less (SF≠OF after cmp)
- Purpose: Convert condition flags to value (1 if true, 0 if false)
- Then used with movzx to move to full register

**Control Flow Instructions (src/codegen.c, line 150-165):**

- **jmp %L1**: Unconditional jump to label L1
  - Pattern: fprintf(output, "jmp %s\n", label) (src/codegen.c, ~156)
  - Implements: IR_JUMP
- **je %L1**: Jump if Equal (if ZF=1)
  - Used after cmp, followed by conditional set
  - Implements: IR_JUMP_IF_ZERO (condition true → jump)
- **jne %L1**: Jump if Not Equal (if ZF=0)
  - Used for opposite condition
  - Implements: IR_JUMP_IF_NOT_ZERO
- **jl %L1**: Jump if Less (if SF≠OF)
  - For less-than comparisons
  - Implements: IR_LT comparisons followed by jump

**Function/Return Instructions (src/codegen.c, line ~160-165):**

- **ret**: Return from function
  - Expects return value in rax
  - Implements: IR_RETURN (pops return address and jumps)
- No explicit push/pop for parameters (simplified calling convention)

**Why These Instructions:**

1. **mov**: Only way to transfer data between memory and registers
2. **add/sub/imul/idiv**: Required for arithmetic (one per operation type)
3. **cmp/setX/jcc**: Required for comparisons and conditionals (standard x86-64 idiom)
4. **jmp/jcc**: Required for loops and if statements (control flow)
5. **ret**: Required for function return
6. **cqo**: Required preparation before signed division
7. **movzx**: Needed to convert condition flags (single bit) to full width value

---

## 8. ERROR HANDLING

**Files:**

- Header: `include/error.h`
- Implementation: `src/error.c` (43 lines)
- Main Entry: `void error_report(ErrorType type, int line, int column, const char *message)` (src/error.c, line 13)

### Question 8.1

**How are errors detected and reported throughout compilation?**

**Answer:**
Comprehensive error handling strategy (src/error.c, line 1-43):

**Error Tracking (src/error.c, line 11-12):**

```c
static int error_count = 0;    // Total errors encountered
static int warning_count = 0;  // Total warnings encountered
```

**Detection Points Across Phases:**

1. **Lexer (src/lexer.c)**: Unknown characters
   - Detects invalid tokens (not keyword, identifier, number, operator)
   - Example: `@` symbol in source code → reported as error

2. **Parser (src/parser.c, line ~1100-1154)**: Grammar violations
   - Detects syntax errors via `parse_error()` calls (src/parser.c, line ~120)
   - Missing semicolons, unmatched braces, unexpected tokens
   - Continues via error recovery/synchronization (skip to next valid point)

3. **Semantic Analysis (src/semantic.c, line 26-253)**: Logical/type errors
   - Undefined variables: `semantic_check_node()` (src/semantic.c, line ~50-100)
   - Redefined symbols: `semantic_check_node()` (src/semantic.c, line ~100-120)
   - Type mismatches: `semantic_infer_type()` (src/semantic.c, line 233)
   - Invalid operations: Applied during type checking

**Universal Reporting Function (src/error.c, line 13-20):**

```c
void error_report(ErrorType type, int line, int column, const char *message)
```

- **type**: Which error category occurred
- **line, column**: Precise location in source code
- **message**: Description of error
- Outputs: `line:column: [ErrorType] message` format
- Example: `5:10: Semantic Error: Undefined variable 'result'`

**Error Reporting Calls:**

- Lexer: `error_report(ERROR_SYNTAX, line, col, "Unknown character")` (src/lexer.c, ~55)
- Parser: `error_report(ERROR_SYNTAX, parser->current_token.line, col, "Unexpected token")` (src/parser.c, ~1120)
- Semantic: `error_report(ERROR_SEMANTIC, node->line, 0, "Undefined symbol")` (src/semantic.c, ~75)

### Question 8.2

**What error types are recognized? (Error Classification)**

**Answer:**
Six error types defined in `include/error.h` (line 1-10):

```c
typedef enum {
    ERROR_SYNTAX,           // line 2
    ERROR_SEMANTIC,         // line 3
    ERROR_TYPE_MISMATCH,    // line 4
    ERROR_UNDEFINED_SYMBOL, // line 5
    ERROR_REDEFINED_SYMBOL, // line 6
    ERROR_INVALID_OPERATION // line 7
} ErrorType;
```

**Descriptions:**

1. **ERROR_SYNTAX** (src/parser.c):
   - What: Grammar/syntax violations
   - When: Parser encounters invalid token sequence
   - Examples: Missing semicolon, unmatched brace, unexpected token
   - Detection: In `parse_error()` calls throughout parser
   - Example: `int x` without semicolon → syntax error

2. **ERROR_SEMANTIC** (src/semantic.c, line ~75):
   - What: Logical errors in code structure
   - When: Semantic analyzer finds structural problems
   - Examples: Duplicate declarations, unexpected statement type
   - Detection: In `semantic_check_node()` traversal
   - Example: Two `int x;` declarations in same scope

3. **ERROR_TYPE_MISMATCH** (src/semantic.c, line ~200-230):
   - What: Type incompatibility in operations
   - When: `semantic_infer_type()` checks operand types don't match operation
   - Examples: Adding string to integer, comparing incompatible types
   - Detection: In type checking logic during semantic analysis
   - Example: `x + "hello"` where x is int

4. **ERROR_UNDEFINED_SYMBOL** (src/semantic.c, line ~50-70):
   - What: Reference to undeclared variable or function
   - When: `symtab_lookup()` fails to find symbol
   - Examples: Using variable before declaration, calling undefined function
   - Detection: Symbol table lookup in semantic analysis
   - Example: `y = undefined_var + 5;` where undefined_var not declared

5. **ERROR_REDEFINED_SYMBOL** (src/semantic.c, line ~100-120):
   - What: Symbol declared multiple times in same scope
   - When: `symtab_define()` finds symbol already exists at current scope level
   - Examples: Two variable declarations with same name, duplicate function definitions
   - Detection: Symbol table insertion check (src/symtab.c, line 50-60)
   - Example: `int x = 5; int x = 10;` in same block

6. **ERROR_INVALID_OPERATION** (src/semantic.c, line ~150-170):
   - What: Operation unsupported for given operand types
   - When: Operation type doesn't match semantic rules
   - Examples: Modulo on non-integers, increment on non-variable
   - Detection: During semantic node walkthrough with type-aware checks
   - Example: `x % "string"` (modulo needs integers)

### Question 8.3

**How does error recovery allow the compiler to continue after errors?**

**Answer:**
Multi-strategy error recovery enables continued compilation after first error:

**1. Parser Synchronization (src/parser.c, line ~1130-1154):**

- **Strategy**: Skip tokens until reaching known recovery point
- **Implementation**: `parser_synchronize()` function (src/parser.c, line ~1130)
- **Procedure**:
  - After reporting parse error, enter recovery mode
  - Skip tokens one by one using `advance_token()` (src/parser.c, line ~60)
  - Stop when encountering top-level keyword or delimiter
  - Resume parsing from next valid statement
- **Result**: Parse rest of file despite earlier error
- **Example**:
  ```c
  int x = ;  // ERROR: Missing expression
  int y = 5; // This still gets parsed due to synchronization
  ```

**2. Semantic Analysis Continuation (src/semantic.c, line 26-40):**

- **Strategy**: Continue walking AST even after errors in earlier nodes
- **Implementation**: Recursive `semantic_check_node()` visits all nodes (src/semantic.c, line ~39)
- **Procedure**:
  - Traverse entire AST depth-first
  - Report error when found, but continue traversal
  - Check all variables even if one undefined
- **Result**: Reports multiple semantic errors in single pass
- **Example**:
  ```c
  int x = y;  // ERROR: y undefined
  int z = w;  // ERROR: w undefined (also reported)
  ```

**3. Error Count Tracking (src/error.c, line 11-12):**

- Global counters track total errors/warnings (src/error.c, line 11-12)
- `error_count++` incremented on each error
- Final return code generated based on total count
- **Result**: Single pass detects all errors, reports comprehensive list

**4. Code Generation with Errors (src/main.c, line ~175-200):**

- Compiler continues through all phases despite errors
- Validation passes (lexer, parser, semantic) mark errors
- Final check: if `error_count > 0`, skip code generation
- **Result**: Prevents generating invalid assembly from partially valid input
- **Example in main.c** (line ~195-200):
  ```c
  if (error_count > 0) {
      return EXIT_FAILURE;  // Skip codegen
  }
  codegen_generate(...);  // Only if no errors
  ```

**5. Benefits vs Step-on-First-Error:**
| Approach | Benefit |
|----------|---------|
| **Continues on Error** | Programmer sees all errors at once, not 1 per recompile |
| **Skips Code Generation** | Prevents generation of invalid executable |
| **Full AST Traversal** | Catches cascading errors from first fault |
| **Line/Column Info** | Precise error location enables quick fixes |

---

## 9. UTILITY FUNCTIONS

**Files:**

- Header: `include/utils.h`
- Implementation: `src/utils.c` (75 lines)
- Utility purpose: **Support infrastructure** for compiler (memory management, string operations, debugging)

### Question 9.1

**What utility functions support the compiler infrastructure?**

**Answer:**
Utility functions organized into three categories (src/utils.c, line 1-75):

**1. Memory Management (src/utils.c, line 10-30):**

- **Purpose**: Safe memory allocation with error checking
- **Functions**:
  - `void *xmalloc(size_t size)` (src/utils.c, line ~12)
    - Wrapper around malloc()
    - Checks for allocation failure
    - Exits program if malloc fails (prevents null pointer derefs)
  - `void *xcalloc(size_t count, size_t size)` (src/utils.c, line ~18)
    - Wrapper around calloc()
    - Allocates and zeros memory
    - Error checking same as xmalloc()
  - `void *xrealloc(void *ptr, size_t size)` (src/utils.c, line ~24)
    - Wrapper around realloc()
    - Resizes existing allocation
    - Error checking same as xmalloc()
  - `void xfree(void **ptr)` (src/utils.c, line ~30)
    - Safe free with null-pointer-to-pointer argument
    - Sets ptr to NULL after freeing (prevents double-free)
    - Pattern: `xfree((void**)&my_ptr);`

**2. String Operations (src/utils.c, line 35-60):**

- **Purpose**: Safe string handling without buffer overflows
- **Functions**:
  - `char *xstrdup(const char *str)` (src/utils.c, line ~36)
    - Duplicate string with automatic allocation
    - Uses xmalloc() internally (src/utils.c, line ~37)
    - Returns newly allocated copy
    - Example: `char *copy = xstrdup("hello");`
  - `char *str_concat(const char *s1, const char *s2)` (src/utils.c, line ~44)
    - Concatenate two strings into new allocation
    - Uses xmalloc() internally (src/utils.c, line ~45)
    - Returns newly allocated concatenated string
    - Example: `char *combined = str_concat("base", ".c");`

**3. Debug Utilities (src/utils.c, line 65-75):**

- **Purpose**: Debugging support during development
- **Functions**:
  - `void debug_print(const char *fmt, ...)` (src/utils.c, line ~66)
    - Printf-like debug output
    - Only enabled if compiled with DEBUG flag
    - Example: `debug_print("Node type: %d\n", node->type);`

### Question 9.2

**How are utility functions used throughout the compiler?**

**Answer:**
Utility function usage patterns across compiler (src/main.c, all source files):

**Memory Management Usage:**

- **Lexer**: `token = xmalloc(sizeof(Token))` (src/lexer.c, ~line 30)
- **Parser state creation**: `parser = xcalloc(1, sizeof(Parser))` (src/parser.c, ~line 50)
- **AST node creation**: All functions use `xmalloc()` for node allocation (src/ast.c, all functions)
  - Example: `ast_create_node()` (src/ast.c, line ~14) allocates with xmalloc
  - Example: `ast_create_number()` (src/ast.c, line ~35) uses xmalloc
- **Symbol table**: Symbols stored in malloc'd array (src/symtab.c, ~line 20)
- **IR generation**: IR structs allocated with xmalloc (src/ir.c, ~lines 35-40)

**String Operations Usage:**

- **File names**: `xstrdup()` copies filenames for error reporting (src/error.c, ~line 15)
- **Symbol names**: `xstrdup()` copies variable/function names into symbol table
  - `symtab_define(name, ...)` uses xstrdup internally (src/symtab.c, line ~55)
- **Label generation**: `str_concat()` creates unique labels like "L0", "L1"
  - Example: Codegen creates `str_concat("L", label_num)` (src/codegen.c, ~line 140)
- **Temp variables**: Similar pattern for temp names like "t0", "t1"

**Error Handling with Utils:**

- Memory deallocation in cleanup: `xfree((void**)&ptr)` resets pointers (src/main.c, ~line 200)
- Prevents double-free bugs in error recovery paths

**Consistency Benefit:**
All compiler components follow same memory management pattern:

- Allocate with xmalloc/xcalloc (automatic error checking)
- Deallocate with xfree (prevents use-after-free)
- Result: Robust memory handling throughout codebase

### Question 9.3

**Why are utility functions important for compiler implementation?**

**Answer:**
Strategic importance of utilities for compiler robustness:

**1. Error Prevention (src/utils.c):**

- **Null pointer prevention**: xmalloc() exits on failure instead of returning NULL
  - Without this: Every allocation would need null check
  - With this: Simplifies all code paths, failures handled centrally
  - Impact: Reduces boilerplate null checks throughout compiler

- **Double-free prevention**: xfree() sets pointer to NULL after freeing
  - Without this: Accidental double-frees cause crashes
  - With this: Accidental second free() is safe (freeing NULL is no-op)
  - Impact: More robust error recovery code

**2. Code Consistency:**

- **All memory management** follows same pattern (xmalloc/xfree)
- **All string operations** use safe functions (xstrdup, str_concat)
- **All debug output** uses debug_print()
- Result: Standardized coding style, easier maintenance

**3. Development Speed:**

- **Less defensive coding**: Don't need null checks after malloc
  - Example: Without utilities: `ptr = malloc(...); if(!ptr) error();`
  - Example: With utilities: `ptr = xmalloc(...);` (error handled internally)
- **Less boilerplate**: Utilities reduce lines of error-handling code
- **Faster debugging**: debug_print() allows quick instrumentation

**4. Resource Management:**

- **Automatic cleanup patterns**: xfree semantics encourage proper cleanup
- **Consistent naming**: All allocation functions prefixed with 'x' (xmalloc, xcalloc, xrealloc, xfree, xstrdup)
- **Makes code auditable**: Easy to find all allocations (search for 'x' prefix)

**5. Compiler Reliability:**

- **Single point of failure handling**: All allocation failures handled by xmalloc
- **Prevents silent failures**: Crashes immediately if out of memory (vs undefined behavior)
- **Simpler error paths**: Parser, semantic analyzer, codegen don't need allocation error handling

---

## 10. COMPILER PHASES - INTEGRATION QUESTIONS

**Files:**

- Main coordinator: `src/main.c` (200+ lines)
- Phase orchestration: lines ~110-200 contain phase calls
- Output control: lines ~50-100 contain file handling
- Command-line parsing: lines ~1-50

### Question 10.1

**What is the complete compilation pipeline and how do components interact?**

**Answer:**
Complete pipeline with file flow (src/main.c, lines 110-200):

```
Source Code (input.c)
    ↓
[LEXER] (line 115-125)
  Files: include/lexer.h, src/lexer.c
  Entry: Token get_next_token(FILE *fp)
    ↓
Token Stream → [PARSER] (line 130-140)
  Files: include/parser.h, src/parser.c
  Entry: ASTNode *parser_parse(Parser *parser)
    ↓
AST → [SEMANTIC ANALYZER] (line 145-155)
  Files: include/semantic.h, src/semantic.c
  Entry: int semantic_analyze(SemanticAnalyzer *analyzer, ASTNode *ast)
  Uses: Symbol Table (include/symtab.h, src/symtab.c)
    ↓
Validated AST → [IR GENERATOR] (line 160-170)
  Files: include/ir.h, src/ir.c
  Entry: void ir_generate(IRGenerator *ir, ASTNode *ast)
    ↓
Three-Address Code → [CODE GENERATOR] (line 175-185)
  Files: include/codegen.h, src/codegen.c
  Entry: int codegen_generate(CodeGenerator *gen, IRGenerator *ir)
    ↓
Assembly Code → Output File (program.s)
  File written: src/main.c, line ~175 creates output filename
```

**Phase Call Sequence (src/main.c, lines 110-200):**

```c
// Compilation phases called in sequence
if (phase >= 0) lexer_phase();   // Phase 0 (line ~115)
if (phase >= 1) parser_phase();  // Phase 1 (line ~130)
if (phase >= 2) semantic_phase(); // Phase 2 (line ~145)
if (phase >= 3) ir_phase();      // Phase 3 (line ~160)
if (phase >= 4) codegen_phase(); // Phase 4 (line ~175)
```

- **Pipeline characteristic**: Single-pass compiler (each phase reads input once)
- **Error handling**: If error_count > 0, skip to next phase header without processing
- **Output**: Each phase can write intermediate output or skip to next phase

### Question 10.2

**How do the Lexer and Parser work together (Token Interface)?**

**Answer:**
Lexer-Parser interaction via **token stream interface** (src/parser.c):

**Lexer Provides (src/lexer.c, line 25):**

- **Token structure** (include/lexer.h, line 19-35):
  ```c
  typedef struct {
      TokenType type;      // KEYWORD, IDENTIFIER, NUMBER, OPERATOR, etc.
      char value[256];     // Actual text: "int", "myVar", "42", "+", etc.
      int line, column;    // Position in source for error reporting
  } Token;
  ```
- **Entry function**: `Token get_next_token(FILE *fp)` (src/lexer.c, line 25)
  - Reads source file
  - Returns one token at a time
  - Tracks line/column position internally (src/lexer.c, line 6-7)

**Parser Requests (src/parser.c, line 60-80):**

- **Lookahead pattern**:
  - Current token examined: `parser->current_token` (src/parser.c, line ~30)
  - Advance to next token: `parser_advance_token()` (src/parser.c, line ~65)
  - Implementation: Calls lexer internally: `parser->current_token = get_next_token(...)` (src/parser.c, line ~70)

**Example Interaction (src/parser.c, line 160-200):**

```c
// Parser in parse_declaration() function (line 160)
if (current_token.type == KEYWORD_INT) {  // Lexer provided token
    advance_token();                       // Request next token from lexer
    token_id = current_token;              // Get identifier name
    advance_token();                       // Move to next token
    expect(SEMICOLON);                     // Check for semicolon
}
// Lexer called 3 times: once for "int", once for identifier, once for ";"
```

**Lookahead for Parsing Decisions (src/parser.c):**

- **1-token lookahead**: Parser only sees current_token, not future tokens
- **Recursive descent drives lookahead**: Next parse\_\*() call implicitly advances
- **Error reporting**: Line/column from token helps pinpoint errors
  - Example: `error(..., token->line, token->column, "...")`

**Benefits of Token Interface:**
| Aspect | Benefit |
|--------|---------|
| **Separation** | Lexer changes don't affect parser structure |
| **Streaming** | Tokens read one-at-a-time, minimal memory per token |
| **Error Location** | Each token knows its source position |
| **Extensibility** | New token types added without parser changes |

### Question 10.3

**How do the Parser and Semantic Analyzer work together (AST Interface)?**

**Answer:**
Parser-Semantic interaction via **AST interface** (src/semantic.c):

**Parser Provides (src/parser.c, line 110):**

- **AST structure** (include/ast.h, line 55-188):
  ```c
  typedef struct ASTNode {
      ASTNodeType type;           // PROGRAM, DECLARATION, EXPRESSION, etc.
      int line, column;           // Source position
      union {
          struct { ... } program; // PROGRAM has declarations
          struct { ... } variable_decl; // VAR_DECL has name, type
          // ... other node types
      } data;
      struct ASTNode *next;       // Linked list of siblings
  } ASTNode;
  ```
- **Entry function**: `ASTNode *parser_parse(Parser *parser)` (src/parser.c, line 110)
  - Returns complete AST (syntactically validated)
  - Tree structure preserves program structure

**Semantic Analyzer Consumes (src/semantic.c, line 26):**

- **Main entry**: `int semantic_analyze(SemanticAnalyzer *analyzer, ASTNode *ast)` (src/semantic.c, line 26)
- **Walks AST recursively**: `semantic_check_node()` (src/semantic.c, line 39)
  - Traverses entire tree depth-first
  - Checks each node for semantic validity
  - Returns error count

**Symbol Table Building (src/semantic.c, line 70-120):**

- **Phase 1**: Declaration gathering (src/semantic.c, line ~70-90)
  - Walk tree looking for VAR_DECL nodes
  - For each declaration: `symtab_define(ast->data.var_decl.name, ...)` (src/symtab.c, line 50)
  - Stores in symbol table (include/symtab.h, line 20-26)
- **Phase 2**: Reference validation (src/semantic.c, line ~95-120)
  - Walk tree looking for USE nodes
  - For each use: `symtab_lookup(name)` (src/symtab.c, line 82)
  - Reports ERROR_UNDEFINED_SYMBOL if not found

**Error Collection (src/semantic.c, line 26-40):**

- **Continues despite errors**: Visits all nodes even after finding errors
- **Reports with line info**: Uses `ast->line` from parser for error location
  - Example: `error_report(ERROR_SEMANTIC, ast->line, 0, "...")` (src/semantic.c, ~line 75)
- **Error count**: Incremented by error_report() (src/error.c, line 19)

**Interface Benefits:**
| Aspect | Benefit |
|--------|---------|
| **Separation** | Parser doesn't need to know about symbol table |
| **Structure** | AST preserves program structure for analysis |
| **Error Location** | Each AST node knows source line for error messages |
| **Validation** | Multiple passes over AST can do different checks |

### Question 10.4

**How do IR and Code Generation work together (IR Interface)?**

**Answer:**
IR-Codegen interaction via **Three-Address Code interface** (src/codegen.c):

**IR Generator Provides (src/ir.c, line 233):**

- **IR structure** (include/ir.h, line 30-58):
  ```c
  typedef enum { IR_ADD, IR_SUB, IR_MUL, ... } IROpType;  // 19 operation types
  typedef struct { IROpType op; IROperand op1, op2, result; } IRInstruction;
  typedef struct {
      IRInstruction instructions[10000];
      int num_instructions;
      int next_label, next_temp;
  } IRGenerator;
  ```
- **Entry function**: `void ir_generate(IRGenerator *ir, ASTNode *ast)` (src/ir.c, line ~233)
  - Walks validated AST from semantic analyzer
  - Generates TAC instructions
  - Returns populated IRGenerator struct

**Code Generator Consumes (src/codegen.c, line 76):**

- **Main entry**: `int codegen_generate(CodeGenerator *gen, IRGenerator *ir)` (src/codegen.c, line 76)
- **Processes instructions**: Iterates through `ir->instructions[]` (src/codegen.c, line ~80-90)
- **Instruction selection**: Maps each IR instruction to assembly
  - Example: `IR_ADD` → `mov operand1, %rax; add operand2, %rax` (src/codegen.c, line ~104)

**Operand Mapping (src/codegen.c, line 90-110):**

```c
// For each IR instruction:
for (int i = 0; i < ir->num_instructions; i++) {
    IRInstruction instr = ir->instructions[i];
    // Convert operand names to assembly
    switch(instr.op) {
        case IR_ADD:
            fprintf(output, "mov %s, %%rax\n", instr.op1.name);
            fprintf(output, "add %s, %%rax\n", instr.op2.name);
            fprintf(output, "mov %%rax, %s\n", instr.result.name);
            break;
        // ... other operation types
    }
}
```

**Control Flow Translation (src/codegen.c, line 150-165):**

- **IR Labels** (e.g., "L0", "L1") → **Assembly Labels** (e.g., "L0:", "L1:")
- **IR Jumps** (IR_JUMP, IR_JUMP_IF_ZERO) → **Assembly Jumps** (jmp, je, jne, jl)
- **Line-by-line mapping**: Each IR instruction produces one or more assembly instructions
  - Example: `IR_DIV` → `cqo; idiv` (two assembly instructions)
  - Example: `IR_LABEL` → `L1:` (one label, no executable instruction)

**Output File Writing (src/codegen.c, line ~76-80):**

```c
codegen->output = fopen(output_filename, "w");  // Open .s file
// ... generate all instructions ...
fprintf(codegen->output, "...assembly...");      // Write assembly
fclose(codegen->output);                         // Close file
```

**Platform Independence (src/ir.c vs src/codegen.c):**

- **IR is platform-independent**: IR instructions don't mention x86-64
  - Could target ARM by changing codegen only
  - Could optimize IR before code generation
  - Could generate multiple backends from same IR
- **Codegen is x86-64 specific**: Only codegen.c knows about rax, mov, add, etc.
- **Separation of concerns**: IR generation logic separate from instruction selection logic

---

## 11. COMPILER MODES FOR TESTING

**Files:**

- Command-line handling: `src/main.c` (lines 1-50, argument parsing)
- Mode control: `src/main.c` (lines 110-200, phase selection)
- Global mode variable: `src/main.c` (line ~25, `int compilation_phase = 4`)

### Question 11.1

**What compilation modes are provided for testing and debugging?**

**Answer:**
Five compilation modes implemented in `src/main.c` (lines 50-100, argument parsing):

**Mode 0: Lexer Phase (src/main.c, line ~115-125)**

```
./compiler -lexer source.c
```

- **What**: Tokenization phase only
- **Command-line flag**: `-lexer` (src/main.c, line ~70)
- **Output**: Token stream displayed to console
- **Example output**:
  ```
  TokenType: KEYWORD_INT, Value: "int", Line: 1, Column: 0
  TokenType: IDENTIFIER, Value: "x", Line: 1, Column: 4
  TokenType: OPERATOR_ASSIGN, Value: "=", Line: 1, Column: 6
  TokenType: NUMBER, Value: "5", Line: 1, Column: 8
  TokenType: SEMICOLON, Value: ";", Line: 1, Column: 9
  ```
- **Uses**: Verify lexer correctly identifies tokens, test token recognition

**Mode 1: Parser Phase (src/main.c, line ~130-140)**

```
./compiler -parser source.c
```

- **What**: Lexer + Parser phases
- **Command-line flag**: `-parser` (src/main.c, line ~75)
- **Output**: AST printed to console (src/ast.c, `ast_print()` function, line ~180)
- **Example output**:
  ```
  PROGRAM
  ├─ VARIABLE_DECL
  │  ├─ NAME: "x"
  │  ├─ TYPE: "int"
  │  └─ INIT: BINARY_OP +
  │     ├─ LEFT: NUMBER 5
  │     └─ RIGHT: NUMBER 3
  ```
- **Uses**: Verify correct AST structure, test grammar rules, debug parse errors

**Mode 2: Semantic Phase (src/main.c, line ~145-155)**

```
./compiler -semantic source.c
```

- **What**: Lexer + Parser + Semantic Analysis phases
- **Command-line flag**: `-semantic` (src/main.c, line ~80)
- **Output**: Semantic errors reported, validated AST displayed
- **Error example**:
  ```
  5:10: Semantic Error: Undefined variable 'undefined_var'
  ```
- **Uses**: Verify symbol table building, test undefined variable detection, test redefinition detection

**Mode 3: IR Generation Phase (src/main.c, line ~160-170)**

```
./compiler -ir source.c
```

- **What**: Lexer + Parser + Semantic + IR phases
- **Command-line flag**: `-ir` (src/main.c, line ~85)
- **Output**: Three-Address Code (TAC) instructions printed (src/ir.c, `ir_print()` function, line 108)
- **Example output**:
  ```
  t0 = 5
  t1 = 3
  t2 = t0 + t1
  x = t2
  ```
- **Uses**: Verify IR generation logic, test intermediate code correctness, visual program flow

**Mode 4: Code Generation (Default) (src/main.c, line ~175-185)**

```
./compiler source.c        # Default: generates source.s
./compiler -codegen source.c
```

- **What**: Full compilation: Lexer + Parser + Semantic + IR + Codegen
- **Command-line flag**: `-codegen` (src/main.c, line ~90) or omitted (default, line ~35)
- **Output**: Assembly file (source.s) with x86-64 code
- **Example output** (source.s):
  ```asm
  .globl main
  main:
    mov $5, %rax
    mov $3, %rcx
    add %rcx, %rax
    mov %rax, -8(%rbp)
    ret
  ```
- **Uses**: Final compilation to assembly, test code generation logic, test assembly output quality

### Question 11.2

**How are these modes useful for debugging the compiler itself (Incremental Testing)?**

**Answer:**
Five-mode testing strategy enables **phase-by-phase validation** (src/main.c, lines 50-100):

**Layered Testing Architecture (src/main.c, lines 110-200):**

```
Mode 0  ← Lexer errors (tokenization problems)
Mode 1  ← Parser errors (grammar, syntax)
Mode 2  ← Semantic errors (undefined symbols, type mismatches)
Mode 3  ← IR errors (intermediate code generation logic)
Mode 4  ← Codegen errors (assembly generation, instruction selection)
```

**Debugging Workflow Example:**

1. **Compiler doesn't compile test.c → Run `-lexer` mode**
   - `./compiler -lexer test.c` (src/main.c, line 115)
   - Outputs token stream
   - Check: Are all tokens recognized correctly?
   - If tokens wrong → Bug in src/lexer.c
   - If tokens correct → Pass to next mode

2. **Tokens look good → Run `-parser` mode**
   - `./compiler -parser test.c` (src/main.c, line 130)
   - Outputs AST (via ast_print() in src/ast.c, line 180)
   - Check: Is AST structure correct? Do parentheses/braces match?
   - If AST wrong → Bug in src/parser.c
   - If AST correct → Pass to next mode

3. **AST looks good → Run `-semantic` mode**
   - `./compiler -semantic test.c` (src/main.c, line 145)
   - Reports semantic errors (src/semantic.c, line 26)
   - Check: Are undefined variables detected? Are redefinitions caught?
   - If semantic detection wrong → Bug in src/semantic.c
   - If semantic analysis correct → Pass to next mode

4. **Semantic checking works → Run `-ir` mode**
   - `./compiler -ir test.c` (src/main.c, line 160)
   - Outputs TAC (via ir_print() in src/ir.c, line 108)
   - Check: Is IR correctly representing program logic?
   - If IR wrong → Bug in src/ir.c (ir_generate_node() function, line 233)
   - If IR correct → Pass to final mode

5. **IR looks right → Run default codegen**
   - `./compiler test.c` (src/main.c, line 175, default mode 4)
   - Outputs test.s (assembly file)
   - Check: Is assembly syntactically correct? Does it match IR?
   - If assembly wrong → Bug in src/codegen.c (instruction selection)
   - If assembly correct → Compilation succeeded!

**Benefits of This Approach:**
| Aspect | Benefit |
|--------|---------|
| **Error Isolation** | Each mode isolates one phase, makes bugs easy to find |
| **Minimal Debugging** | Don't debug entire compiler, debug one component |
| **Visual Inspection** | Can read token stream, AST, IR directly (human-readable) |
| **Fast Iteration** | Test each phase independently without full compilation |
| **Regression Testing** | Can test each phase on multiple files to catch regressions |

**Mode Implementation Details (src/main.c):**

```c
// Near line 35-50: Parse command-line arguments
if (argc > 1) {
    if (strcmp(argv[1], "-lexer") == 0) compilation_phase = 0;
    else if (strcmp(argv[1], "-parser") == 0) compilation_phase = 1;
    else if (strcmp(argv[1], "-semantic") == 0) compilation_phase = 2;
    else if (strcmp(argv[1], "-ir") == 0) compilation_phase = 3;
    else if (strcmp(argv[1], "-codegen") == 0) compilation_phase = 4;
    else if (argv[1][0] != '-') input_file = argv[1];  // Positional filename
}

// Near line 110-200: Execute up to selected phase
if (phase >= 0) { do_lexer_phase(); }      // Phase 0
if (phase >= 1) { do_parser_phase(); }     // Phase 1 (uses tokens from phase 0)
if (phase >= 2) { do_semantic_phase(); }   // Phase 2 (uses AST from phase 1)
if (phase >= 3) { do_ir_phase(); }         // Phase 3 (uses validated AST)
if (phase >= 4) { do_codegen_phase(); }    // Phase 4 (uses IR from phase 3)
```

**Practical Testing Examples:**

Test file: `int x = 5;`

- `./compiler examples/test_lexer.c` → Outputs assembly to test_lexer.s
- `./compiler -ir examples/test_lexer.c` → Outputs IR to console
- `./compiler -semantic examples/test_lexer.c` → Checks for semantic errors (none expected)

Test file: `int x; int x;` (duplicate variable)

- `./compiler -semantic examples/test_dup_var.c` → Reports ERROR_REDEFINED_SYMBOL (src/semantic.c, line ~100)

Test file: `int x = y;` (undefined variable y)

- `./compiler -semantic examples/test_undef_var.c` → Reports ERROR_UNDEFINED_SYMBOL (src/semantic.c, line ~70)
- **Visual debugging**: See actual output at each stage

---

## 12. PRACTICAL APPLICATION SCENARIOS

**Files:** All compiler phases (src/_.c, include/_.h)
**Usage:** Command-line interface with modes provided in `src/main.c`

### Question 12.1

**How would you compile and debug a simple program like `test_modulo.c`?**

**Answer:**
Complete debugging workflow with file references (src/main.c, line 50-200):

**Step 1: Verify Lexical Analysis (src/lexer.c, line 25)**

```bash
./compiler -lexer examples/test_modulo.c
```

- **Purpose**: Confirm tokenization working correctly
- **Output**: Token stream showing each token type and position
- **Checks**: Are all operators (+, -, \*, /, %) tokenized correctly?
- **Example output**:
  ```
  TokenType: KEYWORD_INT, Value: "int", Line: 1
  TokenType: IDENTIFIER, Value: "result", Line: 1
  ...
  TokenType: OPERATOR_MOD, Value: "%", Line: 3
  ...
  ```
- **If fails**: Debug in src/lexer.c (check is_keyword, operator recognition, line 9-69)

**Step 2: Verify Parsing (src/parser.c, line 110)**

```bash
./compiler -parser examples/test_modulo.c
```

- **Purpose**: Verify AST structure is correct (grammar validation)
- **Output**: AST printed (see src/ast.c, ast_print() function, line 180)
- **Checks**: Do parentheses/braces match? Is tree nested correctly? Are all statements present?
- **Example output**:
  ```
  PROGRAM
  ├─ DECLARATION
  │  ├─ name: result
  │  └─ value: BINARY_OP %
  │     ├─ left: BINARY_OP *
  │     │  ├─ left: NUMBER 10
  │     │  └─ right: NUMBER 3
  │     └─ right: NUMBER 4
  ```
- **If fails**: Debug in src/parser.c (check parse_expression, parse_term, operator precedence, line 160-750)

**Step 3: Verify Semantic Analysis (src/semantic.c, line 26)**

```bash
./compiler -semantic examples/test_modulo.c
```

- **Purpose**: Check for semantic errors (undefined variables, redefinitions, type mismatches)
- **Output**: Error messages (if any) plus validated AST
- **Checks**: Are variables used before declaration? Are there redefinitions? Do operations type-check?
- **Error examples**:
  ```
  3:5: Semantic Error: Undefined variable 'undefined_var'
  5:1: Semantic Error: Redefined symbol 'result'
  ```
- **If semantic errors**: Review src/semantic.c (semantic_check_node, type inference, line 26-253)
- **If no errors**: Proceed to next step

**Step 4: Verify IR Generation (src/ir.c, line 233)**

```bash
./compiler -ir examples/test_modulo.c
```

- **Purpose**: Confirm intermediate code correctly represents program logic
- **Output**: Three-address code listing (see src/ir.c, ir_print(), line 108)
- **Checks**: Are operations in correct order? Are temporaries used correctly? Are labels for jumps correct?
- **Example output**:
  ```
  t0 = 10
  t1 = 3
  t2 = t0 * t1
  t3 = 4
  t4 = t2 % t3
  result = t4
  ```
- **If IR wrong**: Debug in src/ir.c (ir_generate_node, post-order traversal, line 233-382)

**Step 5: Full Compilation and Assembly Review (src/codegen.c, line 76)**

```bash
./compiler examples/test_modulo.c
cat examples/test_modulo.s
```

- **Purpose**: Generate final assembly and verify code generation
- **Output**: x86-64 assembly file (test_modulo.s)
- **Checks**: Is assembly syntactically valid? Do instructions match IR? Is operation order correct?
- **Example output** (test_modulo.s):
  ```asm
  mov $10, %rax
  imul $3, %rax         # t2 = t0 * t1
  mov %rax, %rcx
  mov $4, %rax
  mov %rcx, %rax
  cqo
  mov $4, %rcx
  idiv %rcx             # Division for modulo
  mov %rdx, -8(%rbp)    # result = rdx (remainder)
  ret
  ```
- **If assembly wrong**: Debug in src/codegen.c (instruction selection, line 90-165)

**Complete Workflow Summary:**

```
test_modulo.c → lexer (-lexer mode) → parser (-parser) → semantic (-semantic) → IR (-ir) → codegen (default) → test_modulo.s
      ↑             ↑                    ↑                  ↑                    ↑                ↑
    Input      src/lexer.c         src/parser.c      src/semantic.c ↓      src/ir.c      src/codegen.c
                                                    src/symtab.c
```

### Question 12.2

**How would you add a new operator (e.g., bitwise AND `&`) to the compiler?**

**Answer:**
Cross-phase implementation using provided example files (examples/ and src/):

**Change 1: Lexer - Recognize `&` token (src/lexer.c, line 80-100)**

```c
// In src/lexer.c, in get_next_token() function
} else if (ch == '&') {
    token->type = OPERATOR_AND;      // Define new TokenType
    token->value[0] = '&';
    token->value[1] = '\0';
}
```

- **Modified file**: `src/lexer.c` (line ~95)
- **Related header**: `include/lexer.h` (add OPERATOR_AND to TokenType enum, line ~8-15)

**Change 2: Parser - Parse binary AND expression (src/parser.c, line 290-380)**

- Already handles binary operators generically via `parse_expression()` (line 290)
- Checks for `&` operator in precedence chain
- **No parser change needed!** (Recursive descent cascades through precedence levels)

**Change 3: AST - Represent AND in binary op (include/ast.h, line 55-188)**

- Already generic `BINARY_OP` node with `operator` field
- `&` stored as operator string just like `+`, `-`, `*`, etc.
- **No AST change needed!**

**Change 4: Semantic Analysis (src/semantic.c, line 200-230)**

```c
// In semantic_infer_type() function (line 233)
} else if (strcmp(op, "&") == 0) {
    // Bitwise AND requires integer operands
    if (left_type == TYPE_INT && right_type == TYPE_INT) {
        return TYPE_INT;
    } else {
        error_report(ERROR_TYPE_MISMATCH, ...);
        return TYPE_ERROR;
    }
}
```

- **Modified file**: `src/semantic.c` (line ~220)
- **Purpose**: Type-check AND operation (operands must be integers)

**Change 5: IR Generation (src/ir.c, line 240-320)**

```c
// Step 1: Add IR_AND to IROpType enum (include/ir.h, line 24-48)
typedef enum {
    ...
    IR_AND,        // Add new instruction type
    ...
} IROpType;

// Step 2: Generate IR_AND instruction (src/ir.c, line 298-314)
case BINARY_OP:
    if (strcmp(node->data.binary_op.operator, "&") == 0) {
        ir_emit(ir, IR_AND, left_temp, right_temp, result);  // Emit AND
    }
```

- **Modified files**:
  - `include/ir.h` (add IR_AND to enum, line ~40)
  - `src/ir.c` (add IR_AND case, line ~310)

**Change 6: Code Generation (src/codegen.c, line 90-165)**

```c
// In codegen_generate_instruction() function (line 90)
case IR_AND:
    fprintf(output, "mov %s, %%rax\n", instr.op1.name);
    fprintf(output, "and %s, %%rax\n", instr.op2.name);     // x86-64 AND instruction
    fprintf(output, "mov %%rax, %s\n", instr.result.name);
    break;
```

- **Modified file**: `src/codegen.c` (line ~130)
- **Assembly instruction used**: `and` (logical/bitwise AND)

**Testing the Change (src/main.c, line 50-200):**

```bash
# Create test file with bitwise AND
echo "int main() { int x = 5; int y = 3; int z = x & y; return z; }" > test_and.c

# Test each phase
./compiler -lexer test_and.c    # Verify & token recognized
./compiler -parser test_and.c   # Verify & parsed in expression
./compiler -semantic test_and.c # Verify type check passes
./compiler -ir test_and.c       # Verify IR_AND instruction generated
./compiler test_and.c           # Verify assembly generated
cat test_and.s                  # View generated assembly
```

**Summary of Changes:**
| Phase | File | Change | Line Ref |
|-------|------|--------|----------|
| Lexer | src/lexer.c | Add `&` → OPERATOR_AND | ~95 |
| Lexer | include/lexer.h | Add OPERATOR_AND enum | ~-15 |
| Parser | — | None needed (generic) | — |
| AST | — | None needed (generic) | — |
| Semantic | src/semantic.c | Type check: INT & INT → INT | ~220 |
| IR | include/ir.h | Add IR_AND to enum | ~40 |
| IR | src/ir.c | Generate IR_AND instruction | ~310 |
| Codegen | src/codegen.c | Emit `and` x86-64 instruction | ~130 |

---

## 13. CHALLENGES AND IMPROVEMENTS

**Files:**

- Analysis applies to: All compiler phases (src/_.c, include/_.h)
- Build system: `Makefile`

### Question 13.1

**What are the main limitations of this compiler implementation?**

**Answer:**
Limitations with file references showing implementation boundaries:

**1. Type System Limitations (include/semantic.h, line 8-12)**

- **Current**: Only 5 basic types supported: int, float, char, double, void
  ```c
  typedef enum { TYPE_INT, TYPE_FLOAT, TYPE_CHAR, TYPE_DOUBLE, TYPE_VOID } DataType;
  ```
- **Missing**:
  - Pointers (`int *p`)
  - Arrays (`int arr[10]`)
  - Structs/unions (`struct Point { int x, y; }`)
  - Function types
- **Impact**: Cannot compile programs using pointers, arrays, or custom types
- **Would require changes**: include/semantic.h (type enum), src/semantic.c (type inference), src/parser.c (pointer/array parsing)

**2. Type Checking (src/semantic.c, line 200-230)**

- **Current**: Basic checking only (undefined symbol, redefinition)
- **Missing**:
  - Strict type mismatch detection: `int x = 3.14` → Should warn, currently allows
  - Function argument type checking
  - Array bounds checking
  - Pointer dereference safety
- **Impact**: Semantic errors allowed through to code generation
- **Would require**: Enhanced semantic_infer_type() (src/semantic.c, line 233)

**3. Register Allocation (src/codegen.c, line 90-165)**

- **Current**: Hardcoded register usage
  - Always uses rax, al, rdx for operations (src/codegen.c, line 104-158)
  - No conflict detection for complex expressions
- **Missing**:
  - Modern register allocator (live range analysis)
  - Register spilling to stack
  - Optimization for register reuse
- **Impact**: Generated code uses more memory accesses than optimal
- **Would require**: Complete codegen rewrite with register allocation data structure

**4. No Optimization Passes (src/ir.c)**

- **Current**: IR generated but not optimized
- **Missing**:
  - Constant folding: `x = 5 + 3` → optimized to `x = 8` before codegen
  - Dead code elimination: Remove unreachable/unused code
  - Common subexpression elimination: `t1 = a+b; t2 = a+b` → eliminate duplicate `a+b`
  - Loop optimizations
- **Framework exists**: IRGenerator structure (include/ir.h, line 30-58) could support optimization passes
- **Would require**: New phase between ir_generate() (src/ir.c, line 233) and codegen_generate() (src/codegen.c, line 76)

**5. Limited Data Types (src/parser.c, line 380-430)**

- **Current**: Parse declarations, but limited support for complex types
- **Missing**:
  - Pointer declaration/use: `int *ptr = &x;`
  - Array declaration: `int arr[100];`
  - Struct declaration: `struct Point { int x, y; };`
  - Function pointers
- **Impact**: Parser rejects any of these constructs (syntax error)
- **Would require**: Extended tokenizer (src/lexer.c), parser rules (src/parser.c), AST nodes (src/ast.c)

**6. No Library Functions (src/main.c doesn't link libraries)**

- **Current**: Compiler only handles user code
- **Missing**:
  - Standard library calls: printf, scanf, malloc, free
  - Linking with libc (C runtime library)
  - Calling conventions for library functions
- **Impact**: Cannot call any standard functions
- **Would require**: Linker support, calling convention implementation (src/codegen.c)

**7. Error Recovery Limitations (src/parser.c, line 1130-1154)**

- **Current**: Synchronization by skipping to next statement keyword
- **Missing**:
  - Panic mode recovery for specific constructs
  - Error productions in grammar
  - Better error messages with suggestions
- **Impact**: Error reporting could be more helpful
- **Would require**: Enhanced parser synchronization logic

**8. Symbol Table Scope Limitations (src/symtab.c, line 31-60)**

- **Current**: Flat array with scope level tracking
  - Linear search: O(n) lookup (src/symtab.c, line 82-90)
  - Maximum 1000 symbols (MAX_SYMBOLS, src/symtab.c, line 8)
- **Missing**:
  - Hash table for O(1) lookup
  - Dynamic symbol count
  - Shadowing support (inner scope redefine outer)
- **Impact**: Slower compilation for large programs, hard limit on symbols
- **Would require**: Hash table implementation (src/symtab.c)

**9. Single-Pass Compilation (src/main.c, line 110-200)**

- **Current**: Each phase reads input once without lookahead
- **Missing**:
  - Forward references (use before declaration)
  - Multi-pass compilation for constructs like macros
- **Impact**: Some valid programs rejected
- **Would require**: Multiple passes or additional symbol table pass

**10. No AST Optimization (src/ast.c)**

- **Current**: AST directly reflects source program structure
- **Missing**:
  - Tree rewriting/transformation passes
  - Constant folding at AST level
  - Dead code removal at AST level
- **Would require**: AST transformation functions

### Question 13.2

**What improvements could be made to the compiler (Ranked by Impact)?**

**Answer:**
Practical improvements with implementation guidance (ranked by impact on functionality):

**Priority 1: Full Pointer Support**

- **Current:** No pointer parsing (src/parser.c, line 380-430)
- **Change needed:**
  - Lexer: Add `*` operator recognition (already partially done, src/lexer.c, line 80-100)
  - Parser: Add pointer declarators in `parse_variable_declaration()` (src/parser.c, line 190-240)
  - Semantic: Add pointer type to TYPE enum (include/semantic.h, line 8-12)
  - IR: Add pointer deref/ref operations (include/ir.h, line 24-48)
  - Codegen: Emit address-of and dereference assembly (src/codegen.c, line 90-165)
- **Impact**: Highest - enables dynamic memory, complex data structures
- **Estimated effort**: 3-4 phases to implement

**Priority 2: Hash Table for Symbol Lookup**

- **Current:** Linear array search O(n) (src/symtab.c, line 82-90)
- **Change needed:**
  - Replace `Symbol symbols[MAX_SYMBOLS]` with hash table (src/symtab.c, line 20)
  - Reimplement `symtab_lookup()` with hash (src/symtab.c, line 82)
  - Reimplement `symtab_define()` with collision handling (src/symtab.c, line 50)
- **Impact**: Moderate - speed improvement for large programs
- **Estimated effort**: 1 phase (localized change)

**Priority 3: Constant Folding Optimization**

- **Current:** No IR optimization passes
- **Change needed:**
  - Add optimization function after `ir_generate()` (src/main.c, line 160)
  - Detect `IR_ASSIGN; IR_ADD` pattern with literal operands
  - Replace with single `IR_ASSIGN` with computed constant
  - Example: `t0 = 5; t1 = 3; t2 = t0 + t1` → `t2 = 8`
- **Impact:** Moderate - faster generated code
- **Estimated effort**: 1-2 passes

**Priority 4: Array Support**

- **Current:** No array parse/semantic (src/parser.c, line 380-430)
- **Change needed:**
  - Lexer: Support `[` `]` tokens (src/lexer.c, line 20-70)
  - Parser: Array declarators + indexing (src/parser.c, line 190-240)
  - Semantic: Array type system (include/semantic.h)
  - Codegen: Array indexing to memory access (src/codegen.c, line 90-165)
- **Impact:** High - enable practical programs
- **Estimated effort**: 3-4 phases

**Priority 5: Improved Error Reporting**

- **Current:** Basic line:column error (src/error.c, line 13-20)
- **Change needed:**
  - Track source line text in error_report() (src/error.c, line 13)
  - Display line with caret under error location
  - Add suggestions for common mistakes
  - Example: `Undefined 'x', did you mean 'X'?`
- **Impact:** Moderate - developer experience
- **Estimated effort**: 1 phase

**Priority 6: Function Calls**

- **Current:** No function call support (src/parser.c, line 290-320)
- **Change needed:**
  - Parser: Parse function calls `foo(arg1, arg2)` (src/parser.c, line 320-370)
  - Semantic: Function type checking (src/semantic.c, line 200-230)
  - IR: Add `IR_CALL` instruction (include/ir.h, line 24-48)
  - Codegen: Function call sequences with calling convention (src/codegen.c, line 90-165)
- **Impact:** Critical - enable modular programming
- **Estimated effort**: 4-5 phases (complete rewrite in some areas)

**Priority 7: Standard Library Integration**

- **Current:** No linking capability
- **Change needed:**
  - Codegen: Link with libc by default (Makefile change)
  - Codegen: Implement calling convention for printf, scanf, etc. (src/codegen.c)
  - Support external function declarations (src/parser.c)
- **Impact:** High - enables I/O and common operations
- **Estimated effort**: 1-2 phases + linker configuration

**Implementation Roadmap Example (6-month timeline):**

```
Month 1-2: Hash table + constant folding (Priority 2,3)
  - Improve compilation speed and optimization
  - Enables testing of other features on larger programs

Month 3-4: Pointer support (Priority 1)
  - Add pointer types, dereference, address-of
  - Essential for complex data structures

Month 5: Array support (Priority 4)
  - Add array declarations and indexing
  - Needed for many practical programs

Month 6: Function calls (Priority 6)
  - Enable modular programming
  - Integrate with standard library
```

---

## 14. PRESENTATION DISCUSSION POINTS

**Files:** All phases (src/_.c, include/_.h), presentation material: `questions.md` (this document)

### Question 14.1

**Why is each compilation phase necessary instead of direct translation (Single-Pass Compiler)?**

**Answer:**
Multi-phase design benefits demonstrated in this compiler (src/main.c, lines 110-200):

**1. Separation of Concerns (Each phase has one responsibility)**

- **Lexer** (src/lexer.c, line 25): Only tokenization
- **Parser** (src/parser.c, line 110): Only syntax analysis
- **Semantic** (src/semantic.c, line 26): Only type/symbol checking
- **IR Gen** (src/ir.c, line 233): Only intermediate representation
- **Codegen** (src/codegen.c, line 76): Only assembly generation
- **Benefit**: Easier to understand, modify, test each phase independently

**2. Modularity (Phases are independently testable)**

- **Testing capability**: Can test each phase with demo modes (src/main.c, lines 70-90)
  - `./compiler -lexer input.c` → Tests just lexer (src/main.c, line 115)
  - `./compiler -parser input.c` → Tests lexer + parser (src/main.c, line 130)
  - `./compiler -semantic input.c` → Tests first 3 phases (src/main.c, line 145)
- **Benefit**: Bugs isolated to specific phase (eliminates cross-phase interaction)
- **Example**: If AST is wrong, know bug is in parser (src/parser.c), not semantic (src/semantic.c)

**3. Reusability (IR is independent of source language)**

- **Platform independence**: Same IR can target different architectures
  - Current: x86-64 codegen (src/codegen.c, line 76)
  - Could add: ARM codegen with same IR (just a new codegen_arm.c)
  - Could add: MIPS codegen with same IR (just a new codegen_mips.c)
  - Symbol table reusable for any language (include/symtab.h)
- **Benefit**: Major compiler update (new target) only affects codegen phase

**4. Maintainability (Focused code is easier to fix)**

- **Bug fixes**: Scope limited to specific phase
- **Example**: Fix for "undefined variable detection" (src/semantic.c, line ~70-90)
  - Only semantic analyzer modified
  - Parser (src/parser.c) unaffected
  - Codegen (src/codegen.c) unaffected
- **Benefit**: Changes in one phase don't require changes in others

**5. Extensibility (Features affect specific phases only)**

- **Adding new operator** (example from Q12.2):
  - Lexer (src/lexer.c): Add token recognition
  - Parser (src/parser.c): Automatic (generic rule)
  - Semantic (src/semantic.c): Add type check
  - IR (include/ir.h + src/ir.c): Add instruction type
  - Codegen (src/codegen.c): Add assembly emission
  - Total change: 4-5 files, localized to each phase
- **Benefit**: Features added systematically, no cross-cutting changes

**6. Error Recovery (Better error handling per phase)**

- **Lexer errors**: Unknown character → skip, report error (src/lexer.c, line ~55)
- **Parser errors**: Unexpected token → synchronize (src/parser.c, line ~1130)
- **Semantic errors**: Report multiple errors in one pass (src/semantic.c, line 26-40)
- **Benefit**: Compiler continues despite errors, reports comprehensive error list
- **Single-pass alternative**: Would stop at first error, requiring recompile for each fix

**7. Optimization Opportunities (Can optimize at multiple levels)**

- **Lexer-level**: Token stream normalization
- **Parser-level**: Grammar-level optimizations
- **AST-level**: Tree reorganization (not done currently)
- **IR-level**: Three-address code optimization (framework exists, not implemented)
  - Could add constant folding (combine `t0=5; t1=t0+3` → `t1=8`)
  - Could add dead code elimination
  - Could add common subexpression elimination
- **Codegen-level**: Instruction-level optimizations (mostly done via IR)
- **Benefit**: Framework allows optimization at many points, improves performance progressively

**8. Debugging and Visibility (Can inspect intermediate outputs)**

- **Token stream visible**: `./compiler -lexer` (src/main.c, line 115)
- **Syntax tree visible**: `./compiler -parser` with ast_print() (src/ast.c, line 180)
- **IR visible**: `./compiler -ir` with ir_print() (src/ir.c, line 108)
- **Benefit**: Programmer can trace program through each phase, understand flow

**Comparison: Direct Translation (Single-Pass Compiler)**
If lexer directly generated assembly (no intermediate phases):

```
Source → Lexer → Codegen → Assembly
```

- ❌ Cannot test tokenization independently
- ❌ Parser and semantic checking mixed with codegen
- ❌ Cannot change target architecture (assembly hardcoded in parser)
- ❌ Error on line 100 requires recompiling and debugging parser+codegen together
- ❌ Optimizations must be done in parser (complex, hard to understand)
- ❌ Total lines of mixed code much higher

### Question 14.2

**How does this compiler compare to real-world compilers like GCC/Clang?**

**Answer:**
Detailed comparison using this compiler as reference (src/main.c, overall structure):

**Similarities: Core Architecture Matches Professional Compilers**
| Aspect | This Compiler | GCC/Clang |
|--------|---|---|
| **Lexer** | Token stream (src/lexer.c, line 25) | Yes, extensive lexer |
| **Parser** | Recursive descent (src/parser.c, line 110) | Recursive descent or LR parser |
| **AST** | Union-based nodes (include/ast.h, line 55-188) | Similar (tree of nodes) |
| **Semantic** | Symbol table, type checking (src/semantic.c, line 26) | Yes, extensive |
| **IR** | Three-address code (include/ir.h, line 30-58) | Yes, LLVM IR or RTL IR |
| **Codegen** | Instruction selection (src/codegen.c, line 90) | Yes, complex instruction selection |
| **Error Reporting** | Line/column (src/error.c, line 13) | Yes, detailed |

**Key Differences: Scope and Sophistication**

**1. Language Support**

- **This compiler**: 1 simple language (subset of C)
  - Variables (int, float, char, double)
  - Binary operations (+, -, \*, /, %)
  - Control flow (if, while, return)
  - Declarations, assignments
- **GCC**: Full C standard + C++ + Go + Objective-C, etc.
- **Impact**: GCC parser (src/parser.c equivalent) is 50,000+ lines; ours is 1,154 lines (src/parser.c)

**2. Type System**

- **This compiler**: 5 basic types (include/semantic.h, line 8-12), basic checking
  - No pointers, arrays, structs
  - No implicit/explicit conversions
- **GCC**: Full C type system with:
  - Pointers (multiple levels)
  - Arrays, structs, unions, enums
  - Type conversions and promotions
  - Function types
- **Impact**: GCC type checking (src/semantic.c equivalent) is 10,000+ lines; ours is 253 lines

**3. Optimization Passes**

- **This compiler**: 0 optimization passes
  - No constant folding
  - No dead code elimination
  - No loop optimization
  - IR generated but immediately translated (src/ir.c line 233 → src/codegen.c line 76)
- **GCC**: 100+ optimization passes
  - SSA (Static Single Assignment) form
  - Common subexpression elimination
  - Loop invariant code motion
  - Vectorization
- **Impact**: GCC -O2 generates 5-10x faster code; -O3 generates 10-20x faster code

**4. Register Allocation**

- **This compiler**: Hardcoded
  - Always uses rax, al, rdx (src/codegen.c, line 104-158)
  - No liveness analysis
  - No graph coloring
- **GCC**: Sophisticated graph coloring
  - Builds interference graph
  - Allocates registers based on live ranges
  - Spills to stack when needed
- **Impact**: GCC code uses registers better, fewer memory accesses

**5. Architecture Support**

- **This compiler**: x86-64 only
  - Hardcoded mov, add, imul, idiv, jne, etc. (src/codegen.c, line 90-165)
- **GCC**: 40+ architectures
  - ARM, MIPS, PowerPC, RISC-V, etc.
  - Each with specialized codegen
- **Impact**: GCC recompiles to any CPU

**6. Linking and Symbols**

- **This compiler**: No linking
  - Only generates assembly for single file
  - No external symbol references
- **GCC**: Full linking support
  - Multiple object files
  - Libraries (static and dynamic)
  - Symbol export/import
- **Impact**: GCC can build multi-file projects

**7. Error Messages**

- **This compiler**: Basic
  - File:line:column message (src/error.c, line 13)
  - Example: `5:10: Semantic Error: Undefined variable 'x'`
- **GCC**: Detailed with context
  - Shows source line with caret
  - Shows similar names ("Did you mean 'X'?")
  - Multiple errors per line
- **Impact**: GCC error messages guide fixes directly

**8. Debug Information**

- **This compiler**: None
  - No debug symbols in assembly
  - Cannot debug with gdb
- **GCC**: Full DWARF debug information
  - Debug symbols in executable
  - Can single-step in debugger (gdb)
  - Can inspect variables at runtime
- **Impact**: GCC executables debuggable

**Complexity: Rough Comparison (Lines of Code)**

```
Phase           | This Compiler | GCC (estimate)
────────────────|───────────────|─────────────────
Lexer           | 408 lines     | 50,000 lines
Parser          | 1,154 lines   | 100,000 lines
AST             | 634 lines     | Built-in tree
Semantic        | 253 lines     | 50,000 lines
Symbol Table    | 105 lines     | Built-in
IR Generation   | 382 lines     | 10,000 lines
Optimization    | 0 lines       | 500,000 lines
Code Generation | 184 lines     | 100,000 lines
────────────────|───────────────|─────────────────
TOTAL: ~3,120   | ~810,000
```

**What This Compiler Does Well**

1. ✅ **Simple**: Easy to understand end-to-end (3,000 lines vs 810,000 lines)
2. ✅ **Educational**: Clear phase separation (src/main.c demonstrates each phase)
3. ✅ **Runnable**: Full working compiler, generates correct assembly
4. ✅ **Debuggable**: Mode-based testing (`-lexer`, `-parser`, `-semantic`, `-ir`, `-codegen`)
5. ✅ **Modular**: Can modify each phase independently

**Where GCC Excels**

1. ✅ Production-ready: Handles full C standard (this handles subset)
2. ✅ Optimizing: Generates 10-100x faster code (this generates straightforward code)
3. ✅ Portable: 40+ architectures (this is x86-64 only)
4. ✅ Debuggable: Includes debug symbols (this doesn't)
5. ✅ Large ecosystem: Works with all C libraries (this works standalone)

**Summary: Educational vs Industrial**
| Aspect | This Compiler | GCC |
|--------|---|---|
| Learn compilation | ✅ Excellent | ❌ Too complex |
| Production use | ❌ Limited | ✅ Industry standard |
| Code readability | ✅ High | ❌ Very low |
| Performance | ❌ Basic | ✅ Excellent |
| Feature coverage | ❌ Subset of C | ✅ Full C + extensions |

---

## Summary Table of Components (Complete Implementation Reference)

| Component          | Files                                                        | Key Functions                                                    | Input               | Output                 | Purpose                                                      |
| ------------------ | ------------------------------------------------------------ | ---------------------------------------------------------------- | ------------------- | ---------------------- | ------------------------------------------------------------ |
| **Lexer**          | `include/lexer.h` (35 lines) `src/lexer.c` (408 lines)       | `get_next_token()` (line 25) `is_keyword()` (line 9)             | Source file         | Token stream           | Tokenization: Convert source characters to meaningful tokens |
| **Parser**         | `include/parser.h` (20 lines) `src/parser.c` (1,154 lines)   | `parser_parse()` (line 110) `parse_expression()` (line 290)      | Tokens              | AST                    | Syntax analysis: Validate grammar and build tree structure   |
| **AST**            | `include/ast.h` (234 lines) `src/ast.c` (634 lines)          | `ast_create_node()` (line 14) `ast_print()` (line 180)           | Parser output       | AST nodes              | Abstract syntax tree: Represents program structure           |
| **Semantic**       | `include/semantic.h` (15 lines) `src/semantic.c` (253 lines) | `semantic_analyze()` (line 26) `semantic_check_node()` (line 39) | AST                 | Validated AST + errors | Type checking: Validate symbols and types                    |
| **Symbol Table**   | `include/symtab.h` (30 lines) `src/symtab.c` (105 lines)     | `symtab_define()` (line 50) `symtab_lookup()` (line 82)          | Declarations        | Symbol info            | Scope management: Track variable definitions and scope       |
| **IR Generator**   | `include/ir.h` (58 lines) `src/ir.c` (382 lines)             | `ir_generate()` (line 233) `ir_emit()` (line 45)                 | AST                 | TAC instructions       | Intermediate representation: Platform-independent code       |
| **Code Generator** | `include/codegen.h` (15 lines) `src/codegen.c` (184 lines)   | `codegen_generate()` (line 76)                                   | IR                  | Assembly (.s)          | Code generation: Translate to x86-64 assembly                |
| **Error Handler**  | `include/error.h` (10 lines) `src/error.c` (43 lines)        | `error_report()` (line 13)                                       | Errors at any phase | Error messages         | Diagnostic reporting: Report errors with locations           |
| **Utilities**      | `include/utils.h` (8 lines) `src/utils.c` (75 lines)         | `xmalloc()` (line 12) `xstrdup()` (line 36)                      | N/A                 | Memory/strings         | Support functions: Memory and string management              |
| **Main**           | `src/main.c` (200+ lines)                                    | `main()`                                                         | Arguments           | Output file            | Orchestration: Coordinate all phases, handle CLI             |

**Total Implementation: ~3,120 lines of C code**

### File Summary by Purpose

**Header Files (include/) - Interface Definitions**: 187 lines total

- Define data structures, function signatures, enums
- Allow phases to communicate through well-defined interfaces

**Source Files (src/) - Implementations**: 2,933 lines total

- Phase implementations (lexer, parser, ast, semantic, ir, codegen)
- Support functions (symtab, error, utils)
- Main orchestration (main.c)

**Test Files (examples/) - Verification**

- test_lexer.c - Lexer testing
- test_parser.c - Parser testing
- test_modulo.c - Modulo operation
- test_advanced.c - Complex expressions

---

## Index of Key Concepts and File Locations

### Parsing Technique

- **TOP-DOWN RECURSIVE DESCENT PARSING** (src/parser.c, line 110-750)
  - 21 parsing functions implementing grammar rules
  - Operator precedence via cascading function calls
  - 1-token lookahead

### Data Structures

- **Token** (include/lexer.h, line 19-35)
- **AST Node** (include/ast.h, line 55-188) - 24 node types
- **Symbol** (include/symtab.h, line 20-26)
- **IR Instruction** (include/ir.h, line 41-45) - 19 instruction types
- **IRGenerator** (include/ir.h, line 54-58)

### Compiler Phases (src/main.c, line 110-200)

1. **Lexer** (Phase 0): src/lexer.c
2. **Parser** (Phase 1): src/parser.c
3. **Semantic** (Phase 2): src/semantic.c
4. **IR Generation** (Phase 3): src/ir.c
5. **Code Generation** (Phase 4): src/codegen.c

### Error Types (include/error.h, line 1-10)

- ERROR_SYNTAX (parser errors)
- ERROR_SEMANTIC (semantic errors)
- ERROR_TYPE_MISMATCH (type errors)
- ERROR_UNDEFINED_SYMBOL (symbol not found)
- ERROR_REDEFINED_SYMBOL (duplicate symbol)
- ERROR_INVALID_OPERATION (unsupported operation)

### Assembly Instructions Generated (src/codegen.c, line 90-165)

- **Data**: mov, movzx
- **Arithmetic**: add, sub, imul, cqo, idiv
- **Comparison**: cmp, setX (sete, setne, setl)
- **Control**: jmp, je, jne, jl, ret
- **x86-64 calling convention**: rax for return value, functions via ret
