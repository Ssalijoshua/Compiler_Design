/*
 * Abstract Syntax Tree (AST) Implementation
 *
 * Implements AST node creation and manipulation functions.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../include/ast.h"

// Create a generic AST node
ASTNode *ast_create_node(NodeType type, int line, int column)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (node == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed for AST node\n");
        return NULL;
    }

    node->type = type;
    node->line = line;
    node->column = column;
    memset(&node->data, 0, sizeof(node->data));

    return node;
}

// Create program node
ASTNode *ast_create_program(ASTNode **declarations, int num)
{
    ASTNode *node = ast_create_node(NODE_PROGRAM, 1, 1);
    if (node != NULL)
    {
        node->data.program.declarations = declarations;
        node->data.program.num_declarations = num;
    }
    return node;
}

// Create function declaration node
ASTNode *ast_create_function_decl(const char *name, DataType return_type,
                                   ASTNode **params, int num_params,
                                   ASTNode *body, int line, int column)
{
    ASTNode *node = ast_create_node(NODE_FUNCTION_DECL, line, column);
    if (node != NULL)
    {
        strncpy(node->data.function_decl.name, name,
                sizeof(node->data.function_decl.name) - 1);
        node->data.function_decl.name[sizeof(node->data.function_decl.name) - 1] = '\0';
        node->data.function_decl.return_type = return_type;
        node->data.function_decl.parameters = params;
        node->data.function_decl.num_parameters = num_params;
        node->data.function_decl.body = body;
    }
    return node;
}

// Create variable declaration node
ASTNode *ast_create_var_decl(const char *name, DataType data_type,
                              int is_array, int array_size,
                              ASTNode *initializer, int line, int column)
{
    ASTNode *node = ast_create_node(NODE_VAR_DECL, line, column);
    if (node != NULL)
    {
        strncpy(node->data.var_decl.name, name,
                sizeof(node->data.var_decl.name) - 1);
        node->data.var_decl.name[sizeof(node->data.var_decl.name) - 1] = '\0';
        node->data.var_decl.data_type = data_type;
        node->data.var_decl.is_array = is_array;
        node->data.var_decl.array_size = array_size;
        node->data.var_decl.initializer = initializer;
    }
    return node;
}

// Create block statement node
ASTNode *ast_create_block(ASTNode **statements, int num, int line, int column)
{
    ASTNode *node = ast_create_node(NODE_BLOCK, line, column);
    if (node != NULL)
    {
        node->data.block.statements = statements;
        node->data.block.num_statements = num;
    }
    return node;
}

// Create if statement node
ASTNode *ast_create_if_stmt(ASTNode *condition, ASTNode *then_branch,
                             ASTNode *else_branch, int line, int column)
{
    ASTNode *node = ast_create_node(NODE_IF_STMT, line, column);
    if (node != NULL)
    {
        node->data.if_stmt.condition = condition;
        node->data.if_stmt.then_branch = then_branch;
        node->data.if_stmt.else_branch = else_branch;
    }
    return node;
}

// Create while loop node
ASTNode *ast_create_while_stmt(ASTNode *condition, ASTNode *body,
                                int line, int column)
{
    ASTNode *node = ast_create_node(NODE_WHILE_STMT, line, column);
    if (node != NULL)
    {
        node->data.while_stmt.condition = condition;
        node->data.while_stmt.body = body;
    }
    return node;
}

// Create for loop node
ASTNode *ast_create_for_stmt(ASTNode *init, ASTNode *condition,
                              ASTNode *increment, ASTNode *body,
                              int line, int column)
{
    ASTNode *node = ast_create_node(NODE_FOR_STMT, line, column);
    if (node != NULL)
    {
        node->data.for_stmt.init = init;
        node->data.for_stmt.condition = condition;
        node->data.for_stmt.increment = increment;
        node->data.for_stmt.body = body;
    }
    return node;
}

// Create return statement node
ASTNode *ast_create_return_stmt(ASTNode *value, int line, int column)
{
    ASTNode *node = ast_create_node(NODE_RETURN_STMT, line, column);
    if (node != NULL)
    {
        node->data.return_stmt.value = value;
    }
    return node;
}

// Create break statement node
ASTNode *ast_create_break_stmt(int line, int column)
{
    return ast_create_node(NODE_BREAK_STMT, line, column);
}

// Create continue statement node
ASTNode *ast_create_continue_stmt(int line, int column)
{
    return ast_create_node(NODE_CONTINUE_STMT, line, column);
}

// Create binary operation node
ASTNode *ast_create_binary_op(const char *operator, ASTNode *left,
                               ASTNode *right, int line, int column)
{
    ASTNode *node = ast_create_node(NODE_BINARY_OP, line, column);
    if (node != NULL)
    {
        strncpy(node->data.binary_op.operator, operator,
                sizeof(node->data.binary_op.operator) - 1);
        node->data.binary_op.operator[sizeof(node->data.binary_op.operator) - 1] = '\0';
        node->data.binary_op.left = left;
        node->data.binary_op.right = right;
    }
    return node;
}

// Create unary operation node
ASTNode *ast_create_unary_op(const char *operator, ASTNode *operand,
                              int line, int column)
{
    ASTNode *node = ast_create_node(NODE_UNARY_OP, line, column);
    if (node != NULL)
    {
        strncpy(node->data.unary_op.operator, operator,
                sizeof(node->data.unary_op.operator) - 1);
        node->data.unary_op.operator[sizeof(node->data.unary_op.operator) - 1] = '\0';
        node->data.unary_op.operand = operand;
    }
    return node;
}

// Create assignment node
ASTNode *ast_create_assignment(ASTNode *target, ASTNode *value,
                                int line, int column)
{
    ASTNode *node = ast_create_node(NODE_ASSIGNMENT, line, column);
    if (node != NULL)
    {
        node->data.assignment.target = target;
        node->data.assignment.value = value;
    }
    return node;
}

// Create function call node
ASTNode *ast_create_function_call(const char *function_name,
                                   ASTNode **arguments, int num_args,
                                   int line, int column)
{
    ASTNode *node = ast_create_node(NODE_FUNCTION_CALL, line, column);
    if (node != NULL)
    {
        strncpy(node->data.function_call.function_name, function_name,
                sizeof(node->data.function_call.function_name) - 1);
        node->data.function_call.function_name[sizeof(node->data.function_call.function_name) - 1] = '\0';
        node->data.function_call.arguments = arguments;
        node->data.function_call.num_arguments = num_args;
    }
    return node;
}

// Create array access node
ASTNode *ast_create_array_access(const char *array_name, ASTNode *index,
                                  int line, int column)
{
    ASTNode *node = ast_create_node(NODE_ARRAY_ACCESS, line, column);
    if (node != NULL)
    {
        strncpy(node->data.array_access.array_name, array_name,
                sizeof(node->data.array_access.array_name) - 1);
        node->data.array_access.array_name[sizeof(node->data.array_access.array_name) - 1] = '\0';
        node->data.array_access.index = index;
    }
    return node;
}

// Create identifier node
ASTNode *ast_create_identifier(const char *name, int line, int column)
{
    ASTNode *node = ast_create_node(NODE_IDENTIFIER, line, column);
    if (node != NULL)
    {
        strncpy(node->data.identifier.name, name,
                sizeof(node->data.identifier.name) - 1);
        node->data.identifier.name[sizeof(node->data.identifier.name) - 1] = '\0';
    }
    return node;
}

// Create integer literal node
ASTNode *ast_create_literal_int(const char *value, int line, int column)
{
    ASTNode *node = ast_create_node(NODE_LITERAL, line, column);
    if (node != NULL)
    {
        strncpy(node->data.literal.value, value,
                sizeof(node->data.literal.value) - 1);
        node->data.literal.value[sizeof(node->data.literal.value) - 1] = '\0';
        node->data.literal.literal_type = LITERAL_INT;
    }
    return node;
}

// Create float literal node
ASTNode *ast_create_literal_float(const char *value, int line, int column)
{
    ASTNode *node = ast_create_node(NODE_LITERAL, line, column);
    if (node != NULL)
    {
        strncpy(node->data.literal.value, value,
                sizeof(node->data.literal.value) - 1);
        node->data.literal.value[sizeof(node->data.literal.value) - 1] = '\0';
        node->data.literal.literal_type = LITERAL_FLOAT;
    }
    return node;
}

// Create string literal node
ASTNode *ast_create_literal_string(const char *value, int line, int column)
{
    ASTNode *node = ast_create_node(NODE_LITERAL, line, column);
    if (node != NULL)
    {
        strncpy(node->data.literal.value, value,
                sizeof(node->data.literal.value) - 1);
        node->data.literal.value[sizeof(node->data.literal.value) - 1] = '\0';
        node->data.literal.literal_type = LITERAL_STRING;
    }
    return node;
}

// Create character literal node
ASTNode *ast_create_literal_char(const char *value, int line, int column)
{
    ASTNode *node = ast_create_node(NODE_LITERAL, line, column);
    if (node != NULL)
    {
        strncpy(node->data.literal.value, value,
                sizeof(node->data.literal.value) - 1);
        node->data.literal.value[sizeof(node->data.literal.value) - 1] = '\0';
        node->data.literal.literal_type = LITERAL_CHAR;
    }
    return node;
}

// Create type cast node
ASTNode *ast_create_cast(DataType target_type, ASTNode *expression,
                          int line, int column)
{
    ASTNode *node = ast_create_node(NODE_CAST, line, column);
    if (node != NULL)
    {
        node->data.cast.target_type = target_type;
        node->data.cast.expression = expression;
    }
    return node;
}

// Free AST node and all its children
void ast_free(ASTNode *node)
{
    if (node == NULL)
        return;

    // Free children based on node type
    switch (node->type)
    {
    case NODE_PROGRAM:
        if (node->data.program.declarations != NULL)
        {
            for (int i = 0; i < node->data.program.num_declarations; i++)
            {
                ast_free(node->data.program.declarations[i]);
            }
            free(node->data.program.declarations);
        }
        break;

    case NODE_FUNCTION_DECL:
        if (node->data.function_decl.parameters != NULL)
        {
            for (int i = 0; i < node->data.function_decl.num_parameters; i++)
            {
                ast_free(node->data.function_decl.parameters[i]);
            }
            free(node->data.function_decl.parameters);
        }
        ast_free(node->data.function_decl.body);
        break;

    case NODE_VAR_DECL:
        ast_free(node->data.var_decl.initializer);
        break;

    case NODE_BLOCK:
        if (node->data.block.statements != NULL)
        {
            for (int i = 0; i < node->data.block.num_statements; i++)
            {
                ast_free(node->data.block.statements[i]);
            }
            free(node->data.block.statements);
        }
        break;

    case NODE_EXPRESSION_STMT:
        ast_free(node->data.expression_stmt.expression);
        break;

    case NODE_IF_STMT:
        ast_free(node->data.if_stmt.condition);
        ast_free(node->data.if_stmt.then_branch);
        ast_free(node->data.if_stmt.else_branch);
        break;

    case NODE_WHILE_STMT:
        ast_free(node->data.while_stmt.condition);
        ast_free(node->data.while_stmt.body);
        break;

    case NODE_FOR_STMT:
        ast_free(node->data.for_stmt.init);
        ast_free(node->data.for_stmt.condition);
        ast_free(node->data.for_stmt.increment);
        ast_free(node->data.for_stmt.body);
        break;

    case NODE_RETURN_STMT:
        ast_free(node->data.return_stmt.value);
        break;

    case NODE_BINARY_OP:
        ast_free(node->data.binary_op.left);
        ast_free(node->data.binary_op.right);
        break;

    case NODE_UNARY_OP:
        ast_free(node->data.unary_op.operand);
        break;

    case NODE_ASSIGNMENT:
        ast_free(node->data.assignment.target);
        ast_free(node->data.assignment.value);
        break;

    case NODE_FUNCTION_CALL:
        if (node->data.function_call.arguments != NULL)
        {
            for (int i = 0; i < node->data.function_call.num_arguments; i++)
            {
                ast_free(node->data.function_call.arguments[i]);
            }
            free(node->data.function_call.arguments);
        }
        break;

    case NODE_ARRAY_ACCESS:
        ast_free(node->data.array_access.index);
        break;

    case NODE_CAST:
        ast_free(node->data.cast.expression);
        break;

    case NODE_BREAK_STMT:
    case NODE_CONTINUE_STMT:
    case NODE_IDENTIFIER:
    case NODE_LITERAL:
        // No children to free
        break;
    }

    free(node);
}

// Print AST (for debugging)
static void print_indent(int indent)
{
    for (int i = 0; i < indent; i++)
        printf("  ");
}

void ast_print(ASTNode *node, int indent)
{
    if (node == NULL)
        return;

    print_indent(indent);

    switch (node->type)
    {
    case NODE_PROGRAM:
        printf("Program (%d declarations)\n", node->data.program.num_declarations);
        for (int i = 0; i < node->data.program.num_declarations; i++)
        {
            ast_print(node->data.program.declarations[i], indent + 1);
        }
        break;

    case NODE_FUNCTION_DECL:
        printf("FunctionDecl: %s returns %s (%d params)\n",
               node->data.function_decl.name,
               datatype_to_str(node->data.function_decl.return_type),
               node->data.function_decl.num_parameters);
        if (node->data.function_decl.body)
            ast_print(node->data.function_decl.body, indent + 1);
        break;

    case NODE_VAR_DECL:
        printf("VarDecl: %s (%s)%s\n",
               node->data.var_decl.name,
               datatype_to_str(node->data.var_decl.data_type),
               node->data.var_decl.is_array ? " []" : "");
        if (node->data.var_decl.initializer)
            ast_print(node->data.var_decl.initializer, indent + 1);
        break;

    case NODE_BLOCK:
        printf("Block (%d statements)\n", node->data.block.num_statements);
        for (int i = 0; i < node->data.block.num_statements; i++)
        {
            ast_print(node->data.block.statements[i], indent + 1);
        }
        break;

    case NODE_EXPRESSION_STMT:
        printf("ExpressionStmt\n");
        ast_print(node->data.expression_stmt.expression, indent + 1);
        break;

    case NODE_IF_STMT:
        printf("IfStmt\n");
        print_indent(indent + 1);
        printf("Condition:\n");
        ast_print(node->data.if_stmt.condition, indent + 2);
        print_indent(indent + 1);
        printf("Then:\n");
        ast_print(node->data.if_stmt.then_branch, indent + 2);
        if (node->data.if_stmt.else_branch)
        {
            print_indent(indent + 1);
            printf("Else:\n");
            ast_print(node->data.if_stmt.else_branch, indent + 2);
        }
        break;

    case NODE_WHILE_STMT:
        printf("WhileStmt\n");
        print_indent(indent + 1);
        printf("Condition:\n");
        ast_print(node->data.while_stmt.condition, indent + 2);
        print_indent(indent + 1);
        printf("Body:\n");
        ast_print(node->data.while_stmt.body, indent + 2);
        break;

    case NODE_FOR_STMT:
        printf("ForStmt\n");
        if (node->data.for_stmt.init)
        {
            print_indent(indent + 1);
            printf("Init:\n");
            ast_print(node->data.for_stmt.init, indent + 2);
        }
        if (node->data.for_stmt.condition)
        {
            print_indent(indent + 1);
            printf("Condition:\n");
            ast_print(node->data.for_stmt.condition, indent + 2);
        }
        if (node->data.for_stmt.increment)
        {
            print_indent(indent + 1);
            printf("Increment:\n");
            ast_print(node->data.for_stmt.increment, indent + 2);
        }
        print_indent(indent + 1);
        printf("Body:\n");
        ast_print(node->data.for_stmt.body, indent + 2);
        break;

    case NODE_RETURN_STMT:
        printf("ReturnStmt\n");
        if (node->data.return_stmt.value)
            ast_print(node->data.return_stmt.value, indent + 1);
        break;

    case NODE_BREAK_STMT:
        printf("BreakStmt\n");
        break;

    case NODE_CONTINUE_STMT:
        printf("ContinueStmt\n");
        break;

    case NODE_BINARY_OP:
        printf("BinaryOp: %s\n", node->data.binary_op.operator);
        ast_print(node->data.binary_op.left, indent + 1);
        ast_print(node->data.binary_op.right, indent + 1);
        break;

    case NODE_UNARY_OP:
        printf("UnaryOp: %s\n", node->data.unary_op.operator);
        ast_print(node->data.unary_op.operand, indent + 1);
        break;

    case NODE_ASSIGNMENT:
        printf("Assignment\n");
        ast_print(node->data.assignment.target, indent + 1);
        ast_print(node->data.assignment.value, indent + 1);
        break;

    case NODE_FUNCTION_CALL:
        printf("FunctionCall: %s (%d args)\n",
               node->data.function_call.function_name,
               node->data.function_call.num_arguments);
        for (int i = 0; i < node->data.function_call.num_arguments; i++)
        {
            ast_print(node->data.function_call.arguments[i], indent + 1);
        }
        break;

    case NODE_ARRAY_ACCESS:
        printf("ArrayAccess: %s\n", node->data.array_access.array_name);
        ast_print(node->data.array_access.index, indent + 1);
        break;

    case NODE_IDENTIFIER:
        printf("Identifier: %s\n", node->data.identifier.name);
        break;

    case NODE_LITERAL:
        printf("Literal: %s\n", node->data.literal.value);
        break;

    case NODE_CAST:
        printf("Cast to %s\n", datatype_to_str(node->data.cast.target_type));
        ast_print(node->data.cast.expression, indent + 1);
        break;
    }
}

// Convert string to DataType
DataType str_to_datatype(const char *str)
{
    if (strcmp(str, "int") == 0)
        return TYPE_INT;
    if (strcmp(str, "float") == 0)
        return TYPE_FLOAT;
    if (strcmp(str, "char") == 0)
        return TYPE_CHAR;
    if (strcmp(str, "double") == 0)
        return TYPE_DOUBLE;
    if (strcmp(str, "void") == 0)
        return TYPE_VOID;
    return TYPE_UNKNOWN;
}

// Convert DataType to string
const char *datatype_to_str(DataType type)
{
    switch (type)
    {
    case TYPE_INT:
        return "int";
    case TYPE_FLOAT:
        return "float";
    case TYPE_CHAR:
        return "char";
    case TYPE_DOUBLE:
        return "double";
    case TYPE_VOID:
        return "void";
    default:
        return "unknown";
    }
}
