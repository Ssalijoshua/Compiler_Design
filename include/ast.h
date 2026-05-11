/*
 * Abstract Syntax Tree (AST) Definitions
 *
 * Defines the data structures that represent the syntactic structure
 * of the C program.
 */

#ifndef AST_H
#define AST_H

#include <stdio.h>

// Forward declarations
typedef struct ASTNode ASTNode;

// Node types for different language constructs
typedef enum
{
    // Top-level
    NODE_PROGRAM,
    NODE_FUNCTION_DECL,
    NODE_VAR_DECL,

    // Statements
    NODE_BLOCK,
    NODE_EXPRESSION_STMT,
    NODE_IF_STMT,
    NODE_WHILE_STMT,
    NODE_FOR_STMT,
    NODE_RETURN_STMT,
    NODE_BREAK_STMT,
    NODE_CONTINUE_STMT,

    // Expressions
    NODE_BINARY_OP,
    NODE_UNARY_OP,
    NODE_ASSIGNMENT,
    NODE_FUNCTION_CALL,
    NODE_ARRAY_ACCESS,
    NODE_IDENTIFIER,
    NODE_LITERAL,
    NODE_CAST,
} NodeType;

// Data type enumeration
typedef enum
{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_DOUBLE,
    TYPE_VOID,
    TYPE_UNKNOWN,
} DataType;

// Generic AST Node structure
typedef struct ASTNode
{
    NodeType type;
    int line;
    int column;
    union
    {
        // NODE_PROGRAM
        struct
        {
            struct ASTNode **declarations; // array of function/variable declarations
            int num_declarations;
        } program;

        // NODE_FUNCTION_DECL
        struct
        {
            char name[256];
            DataType return_type;
            struct ASTNode **parameters; // array of parameter declarations
            int num_parameters;
            struct ASTNode *body; // NODE_BLOCK
        } function_decl;

        // NODE_VAR_DECL
        struct
        {
            char name[256];
            DataType data_type;
            int is_array;
            int array_size; // 0 if not an array
            struct ASTNode *initializer; // optional initializer expression
        } var_decl;

        // NODE_BLOCK
        struct
        {
            struct ASTNode **statements; // array of statements
            int num_statements;
        } block;

        // NODE_EXPRESSION_STMT
        struct
        {
            struct ASTNode *expression;
        } expression_stmt;

        // NODE_IF_STMT
        struct
        {
            struct ASTNode *condition;
            struct ASTNode *then_branch;
            struct ASTNode *else_branch; // optional
        } if_stmt;

        // NODE_WHILE_STMT
        struct
        {
            struct ASTNode *condition;
            struct ASTNode *body;
        } while_stmt;

        // NODE_FOR_STMT
        struct
        {
            struct ASTNode *init;      // optional
            struct ASTNode *condition; // optional
            struct ASTNode *increment; // optional
            struct ASTNode *body;
        } for_stmt;

        // NODE_RETURN_STMT
        struct
        {
            struct ASTNode *value; // optional, NULL for void return
        } return_stmt;

        // NODE_BINARY_OP
        struct
        {
            char operator[10]; // "+", "-", "*", "/", "==", "!=", etc.
            struct ASTNode *left;
            struct ASTNode *right;
        } binary_op;

        // NODE_UNARY_OP
        struct
        {
            char operator[10]; // "-", "+", "!", etc.
            struct ASTNode *operand;
        } unary_op;

        // NODE_ASSIGNMENT
        struct
        {
            struct ASTNode *target;     // left-hand side
            struct ASTNode *value;      // right-hand side
        } assignment;

        // NODE_FUNCTION_CALL
        struct
        {
            char function_name[256];
            struct ASTNode **arguments; // array of argument expressions
            int num_arguments;
        } function_call;

        // NODE_ARRAY_ACCESS
        struct
        {
            char array_name[256];
            struct ASTNode *index;
        } array_access;

        // NODE_IDENTIFIER
        struct
        {
            char name[256];
        } identifier;

        // NODE_LITERAL
        struct
        {
            char value[256];     // "10", "3.14", "\"hello\"", etc.
            enum
            {
                LITERAL_INT,
                LITERAL_FLOAT,
                LITERAL_STRING,
                LITERAL_CHAR,
            } literal_type;
        } literal;

        // NODE_CAST
        struct
        {
            DataType target_type;
            struct ASTNode *expression;
        } cast;
    } data;
} ASTNode;

// AST creation functions
ASTNode *ast_create_node(NodeType type, int line, int column);
ASTNode *ast_create_program(ASTNode **declarations, int num);
ASTNode *ast_create_function_decl(const char *name, DataType return_type,
                                   ASTNode **params, int num_params,
                                   ASTNode *body, int line, int column);
ASTNode *ast_create_var_decl(const char *name, DataType data_type,
                              int is_array, int array_size,
                              ASTNode *initializer, int line, int column);
ASTNode *ast_create_block(ASTNode **statements, int num, int line, int column);
ASTNode *ast_create_if_stmt(ASTNode *condition, ASTNode *then_branch,
                             ASTNode *else_branch, int line, int column);
ASTNode *ast_create_while_stmt(ASTNode *condition, ASTNode *body,
                                int line, int column);
ASTNode *ast_create_for_stmt(ASTNode *init, ASTNode *condition,
                              ASTNode *increment, ASTNode *body,
                              int line, int column);
ASTNode *ast_create_return_stmt(ASTNode *value, int line, int column);
ASTNode *ast_create_break_stmt(int line, int column);
ASTNode *ast_create_continue_stmt(int line, int column);
ASTNode *ast_create_binary_op(const char *operator, ASTNode *left,
                               ASTNode *right, int line, int column);
ASTNode *ast_create_unary_op(const char *operator, ASTNode *operand,
                              int line, int column);
ASTNode *ast_create_assignment(ASTNode *target, ASTNode *value,
                                int line, int column);
ASTNode *ast_create_function_call(const char *function_name,
                                   ASTNode **arguments, int num_args,
                                   int line, int column);
ASTNode *ast_create_array_access(const char *array_name, ASTNode *index,
                                  int line, int column);
ASTNode *ast_create_identifier(const char *name, int line, int column);
ASTNode *ast_create_literal_int(const char *value, int line, int column);
ASTNode *ast_create_literal_float(const char *value, int line, int column);
ASTNode *ast_create_literal_string(const char *value, int line, int column);
ASTNode *ast_create_literal_char(const char *value, int line, int column);
ASTNode *ast_create_cast(DataType target_type, ASTNode *expression,
                          int line, int column);

// AST manipulation functions
void ast_free(ASTNode *node);
void ast_print(ASTNode *node, int indent);

// Helper function to convert string to DataType
DataType str_to_datatype(const char *str);
const char *datatype_to_str(DataType type);

#endif
