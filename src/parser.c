/*
 * Parser Implementation
 *
 * Recursive descent parser that builds an Abstract Syntax Tree from tokens.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../include/parser.h"
#include "../include/ast.h"

// Forward declarations of parsing functions
static ASTNode *parse_declaration(Parser *parser);
static ASTNode *parse_variable_declaration(Parser *parser);
static ASTNode *parse_statement(Parser *parser);
static ASTNode *parse_block_statement(Parser *parser);
static ASTNode *parse_if_statement(Parser *parser);
static ASTNode *parse_while_statement(Parser *parser);
static ASTNode *parse_for_statement(Parser *parser);
static ASTNode *parse_return_statement(Parser *parser);
static ASTNode *parse_expression_statement(Parser *parser);
static ASTNode *parse_expression(Parser *parser);
static ASTNode *parse_assignment_expression(Parser *parser);
static ASTNode *parse_logical_or_expression(Parser *parser);
static ASTNode *parse_logical_and_expression(Parser *parser);
static ASTNode *parse_equality_expression(Parser *parser);
static ASTNode *parse_relational_expression(Parser *parser);
static ASTNode *parse_additive_expression(Parser *parser);
static ASTNode *parse_multiplicative_expression(Parser *parser);
static ASTNode *parse_unary_expression(Parser *parser);
static ASTNode *parse_postfix_expression(Parser *parser);
static ASTNode *parse_primary_expression(Parser *parser);

// Helper functions
static Token advance_token(Parser *parser);
static int check_token(Parser *parser, TokenType type);
static void synchronize(Parser *parser);

// Initialize parser
Parser *parser_create(FILE *fp)
{
    Parser *parser = (Parser *)malloc(sizeof(Parser));
    if (parser == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed for parser\n");
        return NULL;
    }

    parser->input_file = fp;
    parser->error_count = 0;
    parser->warning_count = 0;

    // Get the first token
    parser->current_token = get_next_token(fp);

    return parser;
}

// Free parser
void parser_free(Parser *parser)
{
    if (parser != NULL)
    {
        if (parser->input_file != NULL)
        {
            fclose(parser->input_file);
        }
        free(parser);
    }
}

// Error reporting
void parser_error(Parser *parser, const char *message)
{
    fprintf(stderr, "Parse Error at line %d, column %d: %s\n",
            parser->current_token.line, parser->current_token.column, message);
    parser->error_count++;
}

void parser_warning(Parser *parser, const char *message)
{
    fprintf(stderr, "Parse Warning at line %d, column %d: %s\n",
            parser->current_token.line, parser->current_token.column, message);
    parser->warning_count++;
}

// Helper function: advance to next token
static Token advance_token(Parser *parser)
{
    Token current = parser->current_token;
    parser->current_token = get_next_token(parser->input_file);
    return current;
}

// Helper function: check if current token matches type
static int check_token(Parser *parser, TokenType type)
{
    return parser->current_token.type == type;
}

// Helper function: synchronize on error
static void synchronize(Parser *parser)
{
    advance_token(parser);

    while (parser->current_token.type != TOKEN_EOF)
    {
        // Stop at top-level keywords
        if (check_token(parser, TOKEN_KEYWORD))
        {
            if (strcmp(parser->current_token.lexeme, "int") == 0 ||
                strcmp(parser->current_token.lexeme, "float") == 0 ||
                strcmp(parser->current_token.lexeme, "char") == 0 ||
                strcmp(parser->current_token.lexeme, "void") == 0 ||
                strcmp(parser->current_token.lexeme, "double") == 0)
            {
                return;
            }
        }

        if (check_token(parser, TOKEN_SEPARATOR) &&
            parser->current_token.lexeme[0] == '}')
        {
            return;
        }

        advance_token(parser);
    }
}

// Main parsing function
ASTNode *parser_parse(Parser *parser)
{
    ASTNode **declarations = NULL;
    int num_declarations = 0;
    int capacity = 10;

    declarations = (ASTNode **)malloc(sizeof(ASTNode *) * capacity);
    if (declarations == NULL)
    {
        parser_error(parser, "Memory allocation failed for declarations");
        return NULL;
    }

    // Parse declarations until EOF
    while (parser->current_token.type != TOKEN_EOF)
    {
        ASTNode *decl = parse_declaration(parser);
        if (decl != NULL)
        {
            if (num_declarations >= capacity)
            {
                capacity *= 2;
                ASTNode **new_decls = (ASTNode **)realloc(declarations,
                                                          sizeof(ASTNode *) * capacity);
                if (new_decls == NULL)
                {
                    parser_error(parser, "Memory allocation failed");
                    return NULL;
                }
                declarations = new_decls;
            }
            declarations[num_declarations++] = decl;
        }
        else
        {
            synchronize(parser);
        }
    }

    return ast_create_program(declarations, num_declarations);
}

// Parse a top-level declaration
static ASTNode *parse_declaration(Parser *parser)
{
    if (parser->current_token.type != TOKEN_KEYWORD)
    {
        parser_error(parser, "Expected type declaration");
        return NULL;
    }

    // Save the type token
    Token type_token = parser->current_token;
    DataType data_type = str_to_datatype(type_token.lexeme);

    if (data_type == TYPE_UNKNOWN)
    {
        parser_error(parser, "Unknown type");
        return NULL;
    }

    advance_token(parser); // consume type

    if (parser->current_token.type != TOKEN_IDENTIFIER)
    {
        parser_error(parser, "Expected identifier");
        return NULL;
    }

    Token function_name = parser->current_token;
    advance_token(parser);

    // Check if this is a function or variable declaration
    if (check_token(parser, TOKEN_SEPARATOR) && 
        strcmp(parser->current_token.lexeme, "(") == 0)
    {
        // This is a function declaration
        advance_token(parser); // consume '('

        // Parse parameters
        ASTNode **parameters = NULL;
        int num_parameters = 0;
        int param_capacity = 5;

        if (!(check_token(parser, TOKEN_SEPARATOR) && 
              strcmp(parser->current_token.lexeme, ")") == 0))
        {
            parameters = (ASTNode **)malloc(sizeof(ASTNode *) * param_capacity);

            while (1)
            {
                if (parser->current_token.type != TOKEN_KEYWORD)
                {
                    parser_error(parser, "Expected parameter type");
                    break;
                }

                DataType param_type = str_to_datatype(parser->current_token.lexeme);
                advance_token(parser);

                if (parser->current_token.type != TOKEN_IDENTIFIER)
                {
                    parser_error(parser, "Expected parameter name");
                    break;
                }

                ASTNode *param = ast_create_var_decl(
                    parser->current_token.lexeme, param_type, 0, 0, NULL,
                    parser->current_token.line, parser->current_token.column);
                advance_token(parser);

                if (num_parameters >= param_capacity)
                {
                    param_capacity *= 2;
                    ASTNode **new_params = (ASTNode **)realloc(parameters,
                                                               sizeof(ASTNode *) * param_capacity);
                    if (new_params == NULL)
                    {
                        parser_error(parser, "Memory allocation failed");
                        break;
                    }
                    parameters = new_params;
                }
                parameters[num_parameters++] = param;

                // Check for comma separator
                if (check_token(parser, TOKEN_SEPARATOR) && 
                    strcmp(parser->current_token.lexeme, ",") == 0)
                {
                    advance_token(parser); // consume ','
                }
                else
                {
                    break;
                }
            }
        }

        // Expect closing paren
        if (!(check_token(parser, TOKEN_SEPARATOR) && 
              strcmp(parser->current_token.lexeme, ")") == 0))
        {
            parser_error(parser, "Expected ')' after parameters");
            return NULL;
        }

        advance_token(parser); // consume ')'

        // Parse function body
        ASTNode *body = parse_block_statement(parser);

        if (body == NULL)
        {
            parser_error(parser, "Expected function body");
            return NULL;
        }

        return ast_create_function_decl(function_name.lexeme, data_type,
                                        parameters, num_parameters, body,
                                        function_name.line, function_name.column);
    }
    else
    {
        // This is a variable declaration - handle the rest
        int is_array = 0;
        int array_size = 0;
        ASTNode *initializer = NULL;

        // Check for array declaration
        if (check_token(parser, TOKEN_SEPARATOR) && 
            strcmp(parser->current_token.lexeme, "[") == 0)
        {
            is_array = 1;
            advance_token(parser);

            if (parser->current_token.type == TOKEN_LITERAL)
            {
                array_size = atoi(parser->current_token.lexeme);
                advance_token(parser);
            }

            if (!(check_token(parser, TOKEN_SEPARATOR) && 
                  strcmp(parser->current_token.lexeme, "]") == 0))
            {
                parser_error(parser, "Expected ']' after array size");
            }
            else
            {
                advance_token(parser);
            }
        }

        // Check for initialization
        if (check_token(parser, TOKEN_OPERATOR) && 
            strcmp(parser->current_token.lexeme, "=") == 0)
        {
            advance_token(parser); // consume '='
            initializer = parse_expression(parser);
        }

        if (!(check_token(parser, TOKEN_SEPARATOR) && 
              strcmp(parser->current_token.lexeme, ";") == 0))
        {
            parser_error(parser, "Expected ';' after variable declaration");
        }
        else
        {
            advance_token(parser);
        }

        return ast_create_var_decl(function_name.lexeme, data_type, is_array, array_size,
                                   initializer, type_token.line, type_token.column);
    }
}

// Parse variable declaration
static ASTNode *parse_variable_declaration(Parser *parser)
{
    Token type_token = parser->current_token;
    DataType data_type = str_to_datatype(type_token.lexeme);

    if (data_type == TYPE_UNKNOWN)
    {
        parser_error(parser, "Unknown variable type");
        return NULL;
    }

    advance_token(parser);

    if (parser->current_token.type != TOKEN_IDENTIFIER)
    {
        parser_error(parser, "Expected variable name");
        return NULL;
    }

    Token var_name = parser->current_token;
    advance_token(parser);

    int line = type_token.line;
    int column = type_token.column;

    ASTNode *initializer = NULL;
    int is_array = 0;
    int array_size = 0;

    // Check for array declaration
    if (check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, "[") == 0)
    {
        is_array = 1;
        advance_token(parser);

        if (parser->current_token.type == TOKEN_LITERAL)
        {
            array_size = atoi(parser->current_token.lexeme);
            advance_token(parser);
        }

        if (!(check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, "]") == 0))
        {
            parser_error(parser, "Expected ']' after array size");
        }
        else
        {
            advance_token(parser);
        }
    }

    // Check for initialization
    if (check_token(parser, TOKEN_OPERATOR) && strcmp(parser->current_token.lexeme, "=") == 0)
    {
        advance_token(parser); // consume '='
        initializer = parse_expression(parser);
    }

    if (!(check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, ";") == 0))
    {
        parser_error(parser, "Expected ';' after variable declaration");
    }
    else
    {
        advance_token(parser);
    }

    return ast_create_var_decl(var_name.lexeme, data_type, is_array, array_size,
                               initializer, line, column);
}

// Parse a statement
static ASTNode *parse_statement(Parser *parser)
{
    if (parser->current_token.type == TOKEN_EOF)
    {
        return NULL;
    }

    // Block statement
    if (check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, "{") == 0)
    {
        return parse_block_statement(parser);
    }

    // If statement
    if (check_token(parser, TOKEN_KEYWORD) && strcmp(parser->current_token.lexeme, "if") == 0)
    {
        return parse_if_statement(parser);
    }

    // While loop
    if (check_token(parser, TOKEN_KEYWORD) && strcmp(parser->current_token.lexeme, "while") == 0)
    {
        return parse_while_statement(parser);
    }

    // For loop
    if (check_token(parser, TOKEN_KEYWORD) && strcmp(parser->current_token.lexeme, "for") == 0)
    {
        return parse_for_statement(parser);
    }

    // Return statement
    if (check_token(parser, TOKEN_KEYWORD) && strcmp(parser->current_token.lexeme, "return") == 0)
    {
        return parse_return_statement(parser);
    }

    // Break statement
    if (check_token(parser, TOKEN_KEYWORD) && strcmp(parser->current_token.lexeme, "break") == 0)
    {
        Token break_token = parser->current_token;
        advance_token(parser);
        if (!(check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, ";") == 0))
        {
            parser_error(parser, "Expected ';' after break");
        }
        else
        {
            advance_token(parser);
        }
        return ast_create_break_stmt(break_token.line, break_token.column);
    }

    // Continue statement
    if (check_token(parser, TOKEN_KEYWORD) && strcmp(parser->current_token.lexeme, "continue") == 0)
    {
        Token continue_token = parser->current_token;
        advance_token(parser);
        if (!(check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, ";") == 0))
        {
            parser_error(parser, "Expected ';' after continue");
        }
        else
        {
            advance_token(parser);
        }
        return ast_create_continue_stmt(continue_token.line, continue_token.column);
    }

    // Variable declaration inside statement
    if (check_token(parser, TOKEN_KEYWORD))
    {
        const char *lexeme = parser->current_token.lexeme;
        if (strcmp(lexeme, "int") == 0 || strcmp(lexeme, "float") == 0 ||
            strcmp(lexeme, "char") == 0 || strcmp(lexeme, "double") == 0 ||
            strcmp(lexeme, "void") == 0)
        {
            return parse_variable_declaration(parser);
        }
    }

    // Expression statement
    return parse_expression_statement(parser);
}

// Parse block statement
static ASTNode *parse_block_statement(Parser *parser)
{
    if (!(check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, "{") == 0))
    {
        parser_error(parser, "Expected '{'");
        return NULL;
    }

    int line = parser->current_token.line;
    int column = parser->current_token.column;
    advance_token(parser); // consume '{'

    ASTNode **statements = NULL;
    int num_statements = 0;
    int capacity = 10;

    statements = (ASTNode **)malloc(sizeof(ASTNode *) * capacity);

    while (!check_token(parser, TOKEN_EOF) &&
           !(check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, "}") == 0))
    {
        ASTNode *stmt = parse_statement(parser);
        if (stmt != NULL)
        {
            if (num_statements >= capacity)
            {
                capacity *= 2;
                ASTNode **new_stmts = (ASTNode **)realloc(statements,
                                                          sizeof(ASTNode *) * capacity);
                if (new_stmts == NULL)
                {
                    parser_error(parser, "Memory allocation failed");
                    return NULL;
                }
                statements = new_stmts;
            }
            statements[num_statements++] = stmt;
        }
    }

    if (!(check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, "}") == 0))
    {
        parser_error(parser, "Expected '}' after block");
    }
    else
    {
        advance_token(parser); // consume '}'
    }

    return ast_create_block(statements, num_statements, line, column);
}

// Parse if statement
static ASTNode *parse_if_statement(Parser *parser)
{
    Token if_token = parser->current_token;
    advance_token(parser); // consume 'if'

    if (!(check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, "(") == 0))
    {
        parser_error(parser, "Expected '(' after if");
        return NULL;
    }

    advance_token(parser); // consume '('

    ASTNode *condition = parse_expression(parser);
    if (condition == NULL)
    {
        parser_error(parser, "Expected condition in if statement");
        return NULL;
    }

    if (!(check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, ")") == 0))
    {
        parser_error(parser, "Expected ')' after if condition");
    }
    else
    {
        advance_token(parser);
    }

    ASTNode *then_branch = parse_statement(parser);
    if (then_branch == NULL)
    {
        parser_error(parser, "Expected statement after if");
        return NULL;
    }

    ASTNode *else_branch = NULL;
    if (check_token(parser, TOKEN_KEYWORD) && strcmp(parser->current_token.lexeme, "else") == 0)
    {
        advance_token(parser); // consume 'else'
        else_branch = parse_statement(parser);
        if (else_branch == NULL)
        {
            parser_error(parser, "Expected statement after else");
            return NULL;
        }
    }

    return ast_create_if_stmt(condition, then_branch, else_branch,
                              if_token.line, if_token.column);
}

// Parse while statement
static ASTNode *parse_while_statement(Parser *parser)
{
    Token while_token = parser->current_token;
    advance_token(parser); // consume 'while'

    if (!(check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, "(") == 0))
    {
        parser_error(parser, "Expected '(' after while");
        return NULL;
    }

    advance_token(parser); // consume '('

    ASTNode *condition = parse_expression(parser);
    if (condition == NULL)
    {
        parser_error(parser, "Expected condition in while statement");
        return NULL;
    }

    if (!(check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, ")") == 0))
    {
        parser_error(parser, "Expected ')' after while condition");
    }
    else
    {
        advance_token(parser);
    }

    ASTNode *body = parse_statement(parser);
    if (body == NULL)
    {
        parser_error(parser, "Expected statement after while");
        return NULL;
    }

    return ast_create_while_stmt(condition, body, while_token.line, while_token.column);
}

// Parse for statement
static ASTNode *parse_for_statement(Parser *parser)
{
    Token for_token = parser->current_token;
    advance_token(parser); // consume 'for'

    if (!(check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, "(") == 0))
    {
        parser_error(parser, "Expected '(' after for");
        return NULL;
    }

    advance_token(parser); // consume '('

    ASTNode *init = NULL;
    if (!(check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, ";") == 0))
    {
        init = parse_expression(parser);
    }

    if (!(check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, ";") == 0))
    {
        parser_error(parser, "Expected ';' after for init");
    }
    else
    {
        advance_token(parser);
    }

    ASTNode *condition = NULL;
    if (!(check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, ";") == 0))
    {
        condition = parse_expression(parser);
    }

    if (!(check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, ";") == 0))
    {
        parser_error(parser, "Expected ';' after for condition");
    }
    else
    {
        advance_token(parser);
    }

    ASTNode *increment = NULL;
    if (!(check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, ")") == 0))
    {
        increment = parse_expression(parser);
    }

    if (!(check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, ")") == 0))
    {
        parser_error(parser, "Expected ')' after for");
    }
    else
    {
        advance_token(parser);
    }

    ASTNode *body = parse_statement(parser);
    if (body == NULL)
    {
        parser_error(parser, "Expected statement in for loop");
        return NULL;
    }

    return ast_create_for_stmt(init, condition, increment, body,
                               for_token.line, for_token.column);
}

// Parse return statement
static ASTNode *parse_return_statement(Parser *parser)
{
    Token return_token = parser->current_token;
    advance_token(parser); // consume 'return'

    ASTNode *value = NULL;
    if (!(check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, ";") == 0))
    {
        value = parse_expression(parser);
    }

    if (!(check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, ";") == 0))
    {
        parser_error(parser, "Expected ';' after return");
    }
    else
    {
        advance_token(parser);
    }

    return ast_create_return_stmt(value, return_token.line, return_token.column);
}

// Parse expression statement
static ASTNode *parse_expression_statement(Parser *parser)
{
    ASTNode *expr = parse_expression(parser);

    if (expr == NULL)
    {
        return NULL;
    }

    if (!(check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, ";") == 0))
    {
        parser_error(parser, "Expected ';' after expression");
    }
    else
    {
        advance_token(parser);
    }

    ASTNode *expr_stmt = ast_create_node(NODE_EXPRESSION_STMT, expr->line, expr->column);
    expr_stmt->data.expression_stmt.expression = expr;
    return expr_stmt;
}

// Parse expression
static ASTNode *parse_expression(Parser *parser)
{
    return parse_assignment_expression(parser);
}

// Parse assignment expression
static ASTNode *parse_assignment_expression(Parser *parser)
{
    ASTNode *expr = parse_logical_or_expression(parser);

    if (expr != NULL && check_token(parser, TOKEN_OPERATOR) &&
        strcmp(parser->current_token.lexeme, "=") == 0)
    {
        Token assign_token = parser->current_token;
        advance_token(parser);

        ASTNode *value = parse_assignment_expression(parser);
        if (value == NULL)
        {
            parser_error(parser, "Expected expression after '='");
            return expr;
        }

        return ast_create_assignment(expr, value, assign_token.line, assign_token.column);
    }

    return expr;
}

// Parse logical OR expression
static ASTNode *parse_logical_or_expression(Parser *parser)
{
    ASTNode *left = parse_logical_and_expression(parser);

    while (left != NULL && check_token(parser, TOKEN_OPERATOR) &&
           (strcmp(parser->current_token.lexeme, "||") == 0))
    {
        Token op_token = parser->current_token;
        char *op = op_token.lexeme;
        advance_token(parser);

        ASTNode *right = parse_logical_and_expression(parser);
        if (right == NULL)
        {
            parser_error(parser, "Expected expression after operator");
            return left;
        }

        left = ast_create_binary_op(op, left, right, op_token.line, op_token.column);
    }

    return left;
}

// Parse logical AND expression
static ASTNode *parse_logical_and_expression(Parser *parser)
{
    ASTNode *left = parse_equality_expression(parser);

    while (left != NULL && check_token(parser, TOKEN_OPERATOR) &&
           strcmp(parser->current_token.lexeme, "&&") == 0)
    {
        Token op_token = parser->current_token;
        char *op = op_token.lexeme;
        advance_token(parser);

        ASTNode *right = parse_equality_expression(parser);
        if (right == NULL)
        {
            parser_error(parser, "Expected expression after operator");
            return left;
        }

        left = ast_create_binary_op(op, left, right, op_token.line, op_token.column);
    }

    return left;
}

// Parse equality expression
static ASTNode *parse_equality_expression(Parser *parser)
{
    ASTNode *left = parse_relational_expression(parser);

    while (left != NULL && check_token(parser, TOKEN_OPERATOR) &&
           (strcmp(parser->current_token.lexeme, "==") == 0 ||
            strcmp(parser->current_token.lexeme, "!=") == 0))
    {
        Token op_token = parser->current_token;
        char *op = op_token.lexeme;
        advance_token(parser);

        ASTNode *right = parse_relational_expression(parser);
        if (right == NULL)
        {
            parser_error(parser, "Expected expression after operator");
            return left;
        }

        left = ast_create_binary_op(op, left, right, op_token.line, op_token.column);
    }

    return left;
}

// Parse relational expression
static ASTNode *parse_relational_expression(Parser *parser)
{
    ASTNode *left = parse_additive_expression(parser);

    while (left != NULL && check_token(parser, TOKEN_OPERATOR) &&
           (strcmp(parser->current_token.lexeme, "<") == 0 ||
            strcmp(parser->current_token.lexeme, ">") == 0 ||
            strcmp(parser->current_token.lexeme, "<=") == 0 ||
            strcmp(parser->current_token.lexeme, ">=") == 0))
    {
        Token op_token = parser->current_token;
        char *op = op_token.lexeme;
        advance_token(parser);

        ASTNode *right = parse_additive_expression(parser);
        if (right == NULL)
        {
            parser_error(parser, "Expected expression after operator");
            return left;
        }

        left = ast_create_binary_op(op, left, right, op_token.line, op_token.column);
    }

    return left;
}

// Parse additive expression
static ASTNode *parse_additive_expression(Parser *parser)
{
    ASTNode *left = parse_multiplicative_expression(parser);

    while (left != NULL && check_token(parser, TOKEN_OPERATOR) &&
           (strcmp(parser->current_token.lexeme, "+") == 0 ||
            strcmp(parser->current_token.lexeme, "-") == 0))
    {
        Token op_token = parser->current_token;
        char *op = op_token.lexeme;
        advance_token(parser);

        ASTNode *right = parse_multiplicative_expression(parser);
        if (right == NULL)
        {
            parser_error(parser, "Expected expression after operator");
            return left;
        }

        left = ast_create_binary_op(op, left, right, op_token.line, op_token.column);
    }

    return left;
}

// Parse multiplicative expression
static ASTNode *parse_multiplicative_expression(Parser *parser)
{
    ASTNode *left = parse_unary_expression(parser);

    while (left != NULL && check_token(parser, TOKEN_OPERATOR) &&
           (strcmp(parser->current_token.lexeme, "*") == 0 ||
            strcmp(parser->current_token.lexeme, "/") == 0 ||
            strcmp(parser->current_token.lexeme, "%") == 0))
    {
        Token op_token = parser->current_token;
        char *op = op_token.lexeme;
        advance_token(parser);

        ASTNode *right = parse_unary_expression(parser);
        if (right == NULL)
        {
            parser_error(parser, "Expected expression after operator");
            return left;
        }

        left = ast_create_binary_op(op, left, right, op_token.line, op_token.column);
    }

    return left;
}

// Parse unary expression
static ASTNode *parse_unary_expression(Parser *parser)
{
    if (check_token(parser, TOKEN_OPERATOR) &&
        (strcmp(parser->current_token.lexeme, "-") == 0 ||
         strcmp(parser->current_token.lexeme, "+") == 0))
    {
        Token op_token = parser->current_token;
        char *op = op_token.lexeme;
        advance_token(parser);

        ASTNode *operand = parse_unary_expression(parser);
        if (operand == NULL)
        {
            parser_error(parser, "Expected expression after unary operator");
            return NULL;
        }

        return ast_create_unary_op(op, operand, op_token.line, op_token.column);
    }

    return parse_postfix_expression(parser);
}

// Parse postfix expression
static ASTNode *parse_postfix_expression(Parser *parser)
{
    ASTNode *expr = parse_primary_expression(parser);

    while (expr != NULL)
    {
        // Function call
        if (check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, "(") == 0)
        {
            if (expr->type == NODE_IDENTIFIER)
            {
                advance_token(parser); // consume '('

                ASTNode **arguments = NULL;
                int num_arguments = 0;
                int capacity = 5;

                if (!(check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, ")") == 0))
                {
                    arguments = (ASTNode **)malloc(sizeof(ASTNode *) * capacity);

                    while (1)
                    {
                        ASTNode *arg = parse_assignment_expression(parser);
                        if (arg == NULL)
                        {
                            parser_error(parser, "Expected argument");
                            break;
                        }

                        if (num_arguments >= capacity)
                        {
                            capacity *= 2;
                            ASTNode **new_args = (ASTNode **)realloc(arguments,
                                                                      sizeof(ASTNode *) * capacity);
                            if (new_args == NULL)
                            {
                                parser_error(parser, "Memory allocation failed");
                                break;
                            }
                            arguments = new_args;
                        }
                        arguments[num_arguments++] = arg;

                        if (!(check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, ",") == 0))
                        {
                            break;
                        }
                        advance_token(parser); // consume ','
                    }
                }

                if (!(check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, ")") == 0))
                {
                    parser_error(parser, "Expected ')' after arguments");
                }
                else
                {
                    advance_token(parser); // consume ')'
                }

                expr = ast_create_function_call(expr->data.identifier.name, arguments,
                                               num_arguments, expr->line, expr->column);
            }
            else
            {
                break;
            }
        }
        // Array access
        else if (check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, "[") == 0)
        {
            if (expr->type == NODE_IDENTIFIER)
            {
                advance_token(parser); // consume '['

                ASTNode *index = parse_expression(parser);
                if (index == NULL)
                {
                    parser_error(parser, "Expected array index");
                    return expr;
                }

                if (!(check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, "]") == 0))
                {
                    parser_error(parser, "Expected ']' after array index");
                }
                else
                {
                    advance_token(parser); // consume ']'
                }

                expr = ast_create_array_access(expr->data.identifier.name, index,
                                              expr->line, expr->column);
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

    return expr;
}

// Parse primary expression
static ASTNode *parse_primary_expression(Parser *parser)
{
    Token token = parser->current_token;

    // Literal
    if (check_token(parser, TOKEN_LITERAL))
    {
        advance_token(parser);

        // Determine literal type
        if (strchr(token.lexeme, '.'))
        {
            return ast_create_literal_float(token.lexeme, token.line, token.column);
        }
        else if (token.lexeme[0] == '"')
        {
            return ast_create_literal_string(token.lexeme, token.line, token.column);
        }
        else if (token.lexeme[0] == '\'')
        {
            return ast_create_literal_char(token.lexeme, token.line, token.column);
        }
        else
        {
            return ast_create_literal_int(token.lexeme, token.line, token.column);
        }
    }

    // Identifier
    if (check_token(parser, TOKEN_IDENTIFIER))
    {
        advance_token(parser);
        return ast_create_identifier(token.lexeme, token.line, token.column);
    }

    // Parenthesized expression
    if (check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, "(") == 0)
    {
        advance_token(parser); // consume '('

        ASTNode *expr = parse_expression(parser);
        if (expr == NULL)
        {
            parser_error(parser, "Expected expression inside parentheses");
            return NULL;
        }

        if (!(check_token(parser, TOKEN_SEPARATOR) && strcmp(parser->current_token.lexeme, ")") == 0))
        {
            parser_error(parser, "Expected ')' after expression");
        }
        else
        {
            advance_token(parser);
        }

        return expr;
    }

    // Error: unexpected token
    parser_error(parser, "Unexpected token in expression");
    return NULL;
}
