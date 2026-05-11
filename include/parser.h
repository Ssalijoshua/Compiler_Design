/*
 * Parser (Syntax Analyzer)
 *
 * Parses the token stream and builds an Abstract Syntax Tree (AST).
 * Implements the C grammar rules using recursive descent parsing.
 */

#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

// Parser state structure
typedef struct
{
    FILE *input_file;
    Token current_token;
    int error_count;
    int warning_count;
} Parser;

// Initialize parser with input file
Parser *parser_create(FILE *fp);

// Free parser resources
void parser_free(Parser *parser);

// Main parsing function - parses entire program
ASTNode *parser_parse(Parser *parser);

// Error reporting
void parser_error(Parser *parser, const char *message);
void parser_warning(Parser *parser, const char *message);

#endif
