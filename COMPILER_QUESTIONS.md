# Compiler Design Components - Presentation Questions & Answers

## 1. LEXICAL ANALYSIS (LEXER)

### Question 1.1

**Where is the Lexer applied in the compiler pipeline?**

**Answer:**
The Lexer is the **first phase** of compilation. It:

- Reads the source code character-by-character
- Converts character sequences into tokens (keywords, identifiers, literals, operators, separators)
- Tracks line and column numbers for error reporting
- Filters out comments and whitespace
- Example: Converts `int x = 10;` into tokens: `[int][x][=][10][;]`

### Question 1.2

**What types of tokens does the Lexer recognize?**

**Answer:**
The Lexer recognizes 7 token types:

1. **TOKEN_KEYWORD** - Reserved words (int, return, if, while, for, etc.)
2. **TOKEN_IDENTIFIER** - Variable/function names (x, count, main, etc.)
3. **TOKEN_LITERAL** - Constants (10, 3.14, "hello", 'a')
4. **TOKEN_OPERATOR** - Arithmetic/logical operators (+, -, \*, /, %, ==, !=, <, >, etc.)
5. **TOKEN_SEPARATOR** - Delimiters ({, }, (, ), ;, ,, [, ])
6. **TOKEN_EOF** - End of file marker
7. **TOKEN_ERROR** - Invalid characters

### Question 1.3

**How does the Lexer handle comments and multi-character operators?**

**Answer:**

- **Single-line comments** (`//`): Skips all characters until newline
- **Block comments** (`/* ... */`): Skips all characters between markers
- **Multi-character operators**: Recognized as single tokens (e.g., `==`, `!=`, `<=`, `>=`)
- The Lexer maintains state to properly count lines and columns during comment skipping

---

## 2. SYNTAX ANALYSIS (PARSER)

### Question 2.1

**Where is the Parser applied and what does it do?**

**Answer:**
The Parser is the **second phase** of compilation. It:

- Takes the token stream from the Lexer
- Builds an **Abstract Syntax Tree (AST)** based on grammar rules
- Implements **recursive descent parsing** with predictive lookahead
- Validates the syntactic structure of the program
- Reports syntax errors with line/column information
- Example: Parses `int x = 10;` into a VAR_DECL node with initializer

### Question 2.2

**What is recursive descent parsing and how is it implemented?**

**Answer:**
Recursive descent parsing:

- Uses recursive functions to match grammar productions
- Each grammar rule has a corresponding parsing function
- Functions match tokens and call other parsing functions for sub-rules
- Example hierarchy:
  ```
  parse_program()
    ├─ parse_declaration()
    │   ├─ parse_function_decl()
    │   └─ parse_variable_declaration()
    └─ parse_statement()
        ├─ parse_block_statement()
        ├─ parse_if_statement()
        ├─ parse_while_statement()
        ├─ parse_for_statement()
        └─ parse_expression_statement()
  ```

### Question 2.3

**What grammar constructs does the Parser handle?**

**Answer:**
The Parser handles:

1. **Declarations**: Functions and variables
2. **Statements**: Blocks, if/else, while, for, return, break, continue
3. **Expressions**: Binary/unary operations, assignments, function calls, array access, literals
4. **Operator precedence**: Handles proper precedence for arithmetic and logical operators
5. **Error recovery**: Uses synchronization to recover from parse errors

---

## 3. ABSTRACT SYNTAX TREE (AST)

### Question 3.1

**What is the AST and how is it structured in this compiler?**

**Answer:**
The AST is a **tree representation of program structure**:

- **Provides abstract view** - ignores irrelevant syntax (comments, semicolons)
- **Node types** (24 types):
  - Top-level: PROGRAM, FUNCTION_DECL, VAR_DECL
  - Statements: BLOCK, IF_STMT, WHILE_STMT, FOR_STMT, RETURN_STMT
  - Expressions: BINARY_OP, UNARY_OP, ASSIGNMENT, FUNCTION_CALL, ARRAY_ACCESS
- **Union-based data structure**: Each node type has specific data fields
- **Supports debugging**: Can be printed in tree format for visualization

### Question 3.2

**How does the AST represent different program constructs?**

**Answer:**
Examples of AST representation:

- **Variable declaration**: `int x = 10;`
  ```
  VAR_DECL (name=x, type=int)
    └─ LITERAL (value=10)
  ```
- **If statement**: `if (x > 5) { return x; }`
  ```
  IF_STMT
    ├─ Condition: BINARY_OP (>)
    │   ├─ IDENTIFIER (x)
    │   └─ LITERAL (5)
    └─ Then: BLOCK
        └─ RETURN_STMT
            └─ IDENTIFIER (x)
  ```

### Question 3.3

**Why is the AST important for subsequent compiler phases?**

**Answer:**
The AST is crucial because:

1. **Semantic Analysis** uses it to check types and symbols
2. **IR Generation** traverses it to create intermediate code
3. **Code Generation** walks it to generate assembly
4. **Error Recovery** is easier on AST than token stream
5. **Structure clarity** - easy to understand and manipulate the program

---

## 4. SEMANTIC ANALYSIS

### Question 4.1

**What does the Semantic Analyzer do and where is it applied?**

**Answer:**
The Semantic Analyzer is the **third phase**. It:

- Validates the **meaning** of the program
- Performs **type checking** (ensuring operations are valid for types)
- **Resolves symbols** (links identifiers to their declarations)
- **Manages scopes** (global, function, block scopes)
- Detects semantic errors (undefined variables, redefined symbols)
- Works on the AST produced by the Parser
- Example: Verifies `x + y` where both x and y are declared as int

### Question 4.2

**How does the Semantic Analyzer handle scopes?**

**Answer:**
Scope management:

- **Scope levels**: Tracks nesting depth (0=global, 1+=function/block scopes)
- **Enter/Exit**: `symtab_enter_scope()` on block/function entry, `symtab_exit_scope()` on exit
- **Scope chain lookup**: Variables searched from current scope up to global
- **Shadowing**: Inner scopes can redefine outer scope variables
- **Cleanup**: Symbols are removed when exiting scope
- Example: In function body, a variable declared in function scope shadows global variable

### Question 4.3

**What semantic errors does the analyzer detect?**

**Answer:**
Detected errors:

1. **Undefined Symbols** - Using variables/functions not declared
2. **Redefined Symbols** - Declaring same variable twice in same scope
3. **Type Mismatches** - Operations on incompatible types (future enhancement)
4. **Undefined Functions** - Calling non-existent functions
5. **Invalid Operations** - Operations that don't make sense for types

---

## 5. SYMBOL TABLE

### Question 5.1

**What is the Symbol Table and how is it used?**

**Answer:**
The Symbol Table is a **data structure tracking all declared symbols**:

- **Stores**: Symbol name, kind (var/function/parameter), type, scope level
- **Capacity**: Up to 1000 symbols
- **Operations**:
  - `symtab_define()` - Add new symbol
  - `symtab_lookup()` - Search in current + outer scopes
  - `symtab_lookup_local()` - Search only current scope
- **Used by**: Semantic analysis, code generation
- **Example entry**: `(name="x", kind=SYMBOL_VAR, type=TYPE_INT, scope_level=1)`

### Question 5.2

**How does the Symbol Table handle multiple scopes?**

**Answer:**
Multi-scope handling:

- **Flat array structure**: Single array with scope_level field
- **Scope tracking**: Each symbol records its scope_level
- **On scope exit**: Symbols from exited scope are removed from table
- **Lookup order**: Searches from most recent (highest scope level) to global (level 0)
- **Prevents conflicts**: Same name allowed in different scopes
- Example:
  ```
  Global scope (0):     x, y
  ├─ Function scope (1): i, j
  └─ Block scope (2):   x (shadows global)
  ```

### Question 5.3

**Where is the Symbol Table applied throughout compilation?**

**Answer:**
Applied in:

1. **Semantic Analysis**: Define symbols during AST walk, lookup for validation
2. **Type Inference**: Retrieve symbol types for type checking
3. **Code Generation**: Look up variable types to generate correct instructions
4. **Error Reporting**: Get symbol info for error messages
5. **Scope Verification**: Ensure variable access is valid in current scope

---

## 6. INTERMEDIATE CODE GENERATION (IR)

### Question 6.1

**What is Intermediate Representation (IR) and why is it used?**

**Answer:**
IR (Three-Address Code) is a **bridge between high-level AST and low-level assembly**:

- **Three-Address Code (TAC)**: Each instruction has max 3 operands
- **Format**: `result = operand1 op operand2`
- **Benefits**:
  - Independent from target architecture
  - Easier to optimize
  - Easier to generate from AST
  - Easier to translate to assembly
- **Example**: `t0 = a + b` (add a and b, store in temp t0)

### Question 6.2

**What types of IR instructions does this compiler generate?**

**Answer:**
IR instruction types (19 types):

1. **Arithmetic**: ADD, SUB, MUL, DIV, MOD
2. **Comparison**: EQ, NE, LT, LE, GT, GE
3. **Control Flow**: LABEL, JUMP, JUMP_IF_ZERO, JUMP_IF_NOT_ZERO
4. **Function**: CALL, PARAM, RETURN
5. **Assignment**: ASSIGN

- All use standardized format: `result = op1 operator op2`
- Operands can be: variables, literals, temporary variables (t0, t1, ...)

### Question 6.3

**How does IR Generation traverse the AST?**

**Answer:**
Recursive IR generation:

- **Visit each AST node**: Depth-first traversal
- **Generate code for children**: Process operands first (post-order)
- **Emit instructions**: For each node type
- **Temporary variables**: Generate unique temps for intermediate values
- **Labels**: Generate unique labels for control flow
- Example: For `y = x + 5`
  ```
  t0 = x        (load x into t0)
  t1 = 5        (load 5 into t1)
  y = t0 + t1   (add, store in y)
  ```

### Question 6.4

**Where is the IR used in the compilation pipeline?**

**Answer:**
IR is used:

1. **After Semantic Analysis**: IR generator takes validated AST
2. **Before Code Generation**: Code gen translates IR to assembly
3. **Optimization**: Could optimize IR before code generation
4. **Debugging**: IR can be printed to understand program flow
5. **Portability**: Same IR can target different architectures

---

## 7. CODE GENERATION

### Question 7.1

**What does the Code Generator do and what output does it produce?**

**Answer:**
The Code Generator is the **final compilation phase**. It:

- **Translates IR to assembly code**: Converts TAC to x86-64 assembly
- **Generates output file**: Creates `.s` assembly file
- **Produces**: Assembly-like instructions with operands
- **Handles**:
  - Register allocation (simplified - uses specific registers)
  - Instruction selection (choose appropriate x86-64 instruction)
  - Function prologue/epilogue
- **Example**: IR `t0 = a + b` becomes:
  ```asm
  mov %a, %rax
  add %b, %rax
  mov %rax, %t0
  ```

### Question 7.2

**How does the Code Generator handle different types of operations?**

**Answer:**
Operation handling (simplified):

1. **Arithmetic Operations**:
   - ADD/SUB/MUL: Load operands → perform op → store result
   - DIV/MOD: Load dividend → sign extend → divide → extract result
2. **Comparisons**: Compare operands → Set flag → Move flag to result
3. **Control Flow**: Generate labels and conditional/unconditional jumps
4. **Function Calls**: Push parameters → call → move return value
5. **Return**: Move return value to rax → ret instruction

### Question 7.3

**What assembly instructions are generated and why?**

**Answer:**
Key generated instructions:

- **mov**: Move/load values between registers and memory
- **add/sub/imul**: Arithmetic operations
- **cmp**: Compare for conditional operations
- **setX**: Set register based on condition flags (sete, setne, setl, etc.)
- **movzx**: Move with zero extension (convert condition to 0/1)
- **jmp**: Unconditional jump
- **je/jne**: Conditional jumps
- **idiv**: Integer division
- **cqo**: Sign extension for division
- **ret**: Return from function

---

## 8. ERROR HANDLING

### Question 8.1

**How are errors detected and reported throughout compilation?**

**Answer:**
Error handling strategy:

1. **Lexer**: Reports unknown characters as TOKEN_ERROR
2. **Parser**: Reports syntax errors with line/column, continues via synchronization
3. **Semantic Analysis**: Reports semantic errors (undefined symbols, redefinitions)
4. **Centralized reporting**: `error_report()` function with error types
5. **Tracking**: Global error_count and warning_count variables
6. **Location info**: Line and column reported for all errors

- **Example**: `Semantic Error at line 5, column 10: Undefined variable 'x'`

### Question 8.2

**What error types are recognized?**

**Answer:**
Error types (6 types):

1. **ERROR_SYNTAX** - Parse/grammar violation
2. **ERROR_SEMANTIC** - Logical error in code
3. **ERROR_TYPE_MISMATCH** - Type incompatibility
4. **ERROR_UNDEFINED_SYMBOL** - Variable/function not declared
5. **ERROR_REDEFINED_SYMBOL** - Symbol declared twice in same scope
6. **ERROR_INVALID_OPERATION** - Invalid operation for operand types

### Question 8.3

**How does error recovery allow the compiler to continue after errors?**

**Answer:**
Error recovery mechanisms:

1. **Synchronization in Parser**:
   - Skip tokens until finding top-level keyword or delimiter
   - Allows parsing rest of program despite errors
2. **Continue on Errors**:
   - Semantic analysis continues even with errors
   - Code generation proceeds for validation
3. **Error Count Tracking**:
   - Tracks total errors encountered
   - Final return code indicates success/failure
4. **Benefits**: Reports multiple errors at once instead of stopping at first

---

## 9. UTILITY FUNCTIONS

### Question 9.1

**What utility functions support the compiler infrastructure?**

**Answer:**
Utility functions provide:

1. **Memory Management**:
   - `xmalloc(), xcalloc(), xrealloc()` - Wrappers with error checking
   - Exit on allocation failure instead of silent failure
2. **String Operations**:
   - `xstrdup()` - Safe string duplication
   - `str_concat()` - String concatenation
3. **Debug Support**:
   - `debug_print()` - Printf-style debug output
4. **Benefits**:
   - Consistent error handling
   - Automatic resource cleanup
   - Better debugging

---

## 10. COMPILER PHASES - INTEGRATION QUESTIONS

### Question 10.1

**What is the complete compilation pipeline and how do components interact?**

**Answer:**
Complete pipeline:

```
Source Code
    ↓
[LEXER] → Token Stream
    ↓
[PARSER] → AST
    ↓
[SEMANTIC ANALYZER] → Validated AST (with Symbol Table)
    ↓
[IR GENERATOR] → Three-Address Code
    ↓
[CODE GENERATOR] → Assembly Code (.s file)
    ↓
Output File
```

### Question 10.2

**How do the Lexer and Parser work together?**

**Answer:**
Lexer-Parser interaction:

- **Lexer produces Token**: One token at a time
- **Parser consumes Token**: Checks current_token, advances for next
- **Lookahead**: Parser looks at current token to decide parsing rule
- **Token stream**: Parser maintains file pointer, calls lexer for next token
- **Example**: Parser sees KEYWORD "int" → decides to parse variable/function declaration

### Question 10.3

**How do the Parser and Semantic Analyzer work together?**

**Answer:**
Parser-Semantic interaction:

- **Parser outputs**: Complete, validated AST (syntactically correct)
- **Semantic input**: Walks the AST tree
- **Symbol tracking**: Builds symbol table while walking AST
- **Error collection**: Gathers semantic errors but doesn't stop parsing
- **Result**: Semantically validated AST ready for IR generation

### Question 10.4

**How do IR and Code Generation work together?**

**Answer:**
IR-Codegen interaction:

- **IR as interface**: Platform-independent intermediate representation
- **Instruction translation**: Each IR instruction → x86-64 assembly
- **Operand mapping**: IR operands map to assembly registers/memory
- **Control flow**: IR labels/jumps → assembly labels/jumps
- **Separation of concerns**: IR hides AST complexity from code gen

---

## 11. COMPILER MODES FOR TESTING

### Question 11.1

**What compilation modes are provided for testing and debugging?**

**Answer:**
Five compilation modes:

1. **-lexer**: Show tokenization of source code
   - Input: Source file
   - Output: Token list with types and positions
2. **-parser**: Show AST after parsing
   - Input: Source file
   - Output: Abstract syntax tree structure
3. **-semantic**: Show semantic analysis results
   - Input: Source file
   - Output: AST with semantic validation
4. **-ir**: Show intermediate code
   - Input: Source file
   - Output: Three-address code listing
5. **-codegen** (default): Complete compilation
   - Input: Source file
   - Output: Assembly code file (.s)

### Question 11.2

**How are these modes useful for debugging the compiler?**

**Answer:**
Debugging benefits:

1. **Lexer testing**: Verify correct tokenization of input
2. **Parser testing**: Verify correct AST structure (no parse errors)
3. **Semantic testing**: Verify symbol resolution and scope handling
4. **IR testing**: Verify intermediate code generation logic
5. **Codegen testing**: Verify final assembly output

- **Layered approach**: Test each phase incrementally
- **Error isolation**: Find which phase has issues
- **Visual debugging**: See actual output at each stage

---

## 12. PRACTICAL APPLICATION SCENARIOS

### Question 12.1

**How would you compile and debug a simple program like `test_modulo.c`?**

**Answer:**
Step-by-step debugging:

1. **Check syntax**: `./compiler -parser test_modulo.c`
   - Verify AST structure is correct
2. **Check semantics**: `./compiler -semantic test_modulo.c`
   - Verify variables are properly defined and scoped
3. **Check IR**: `./compiler -ir test_modulo.c`
   - Verify intermediate code logic
4. **Full compile**: `./compiler test_modulo.c`
   - Generate final assembly
5. **Review output**: `cat test_modulo.s`
   - Check assembly is correct

### Question 12.2

**How would you add a new operator (e.g., bitwise AND) to the compiler?**

**Answer:**
Changes needed in each phase:

1. **Lexer**: Add `&` to operator recognition (already done)
2. **Parser**: Add parsing rule for binary operator (already generic)
3. **AST**: Already supports BINARY_OP with any operator (no change)
4. **Semantic**: Type checking for & operation
5. **IR**: Add `IR_AND` instruction type
6. **Codegen**: Generate `and %op1, %op2` assembly instruction

---

## 13. CHALLENGES AND IMPROVEMENTS

### Question 13.1

**What are the main limitations of this compiler implementation?**

**Answer:**
Current limitations:

1. **Type System**: Simplified (only int, float, char, double, void)
2. **Type Checking**: Minimal - doesn't fully validate type compatibility
3. **Register Allocation**: Simplified - uses hardcoded registers
4. **Optimization**: No optimization passes on IR
5. **Data Types**: No structs, unions, pointers (partially supported)
6. **Library Functions**: No standard library integration
7. **Error Recovery**: Uses synchronization but could be more sophisticated

### Question 13.2

**What improvements could be made to the compiler?**

**Answer:**
Possible enhancements:

1. **Better Type System**: Full type checking with implicit conversions
2. **Optimization Passes**:
   - Constant folding
   - Dead code elimination
   - Loop optimization
3. **Register Allocation**: Graph coloring or other algorithms
4. **Pointer Support**: Full pointer arithmetic and dereferencing
5. **Standard Library**: Built-in function support
6. **Debug Info**: DWARF debug symbols
7. **Warnings**: More detailed warning messages
8. **Multi-file Support**: Link multiple compilation units

---

## 14. PRESENTATION DISCUSSION POINTS

### Question 14.1

**Why is each compilation phase necessary instead of direct translation?**

**Answer:**
Multi-phase benefits:

1. **Separation of Concerns**: Each phase has single responsibility
2. **Modularity**: Phases can be tested independently
3. **Reusability**: IR is independent of source/target language
4. **Maintainability**: Easier to fix bugs in specific phase
5. **Extensibility**: Adding features affects specific phases only
6. **Error Recovery**: Better error handling per phase
7. **Optimization**: Opportunities to optimize at different levels

### Question 14.2

**How does this compiler compare to real-world compilers like GCC?**

**Answer:**
Similarities:

- Basic pipeline: Lexer → Parser → AST → Semantic → IR → Codegen
- Error reporting with locations
- Symbol tables for scope management
- Intermediate representation

Key differences:

- **Scope**: GCC handles full C standard, this handles subset
- **Optimization**: GCC has extensive optimization passes, this has none
- **Complexity**: GCC is millions of lines, this is ~3000 lines
- **Target**: GCC supports multiple architectures, this targets x86-64 only
- **Features**: Full C vs. simplified subset (no pointers, structs, etc.)

---

## Summary Table of Components

| Component      | Input        | Output             | Key Function                  |
| -------------- | ------------ | ------------------ | ----------------------------- |
| Lexer          | Source code  | Tokens             | Tokenization                  |
| Parser         | Tokens       | AST                | Syntax validation             |
| Semantic       | AST          | Validated AST      | Type/symbol checking          |
| Symbol Table   | Declarations | Symbol info        | Scope management              |
| IR Generator   | AST          | Three-address code | Intermediate representation   |
| Code Generator | IR           | Assembly code      | Platform-specific translation |
| Error Handler  | Errors       | Error messages     | Diagnostic reporting          |
