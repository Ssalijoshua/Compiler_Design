/*
 * Main Compiler Driver
 *
 * Entry point for the compiler.
 * Orchestrates the compilation pipeline: lexing -> parsing -> semantic analysis -> IR -> code generation.
 */

#include <stdio.h>
#include <stdlib.h>
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/semantic.h"
#include "../include/ir.h"
#include "../include/codegen.h"
#include "../include/error.h"

void print_token_type(TokenType type)
{
    switch (type)
    {
    case TOKEN_KEYWORD:
        printf("KEYWORD");
        break;
    case TOKEN_IDENTIFIER:
        printf("IDENTIFIER");
        break;
    case TOKEN_LITERAL:
        printf("LITERAL");
        break;
    case TOKEN_OPERATOR:
        printf("OPERATOR");
        break;
    case TOKEN_SEPARATOR:
        printf("SEPARATOR");
        break;
    case TOKEN_EOF:
        printf("EOF");
        break;
    case TOKEN_ERROR:
        printf("ERROR");
        break;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <source_file>\n", argv[0]);
        printf("Example: %s examples/test_lexer.c\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Error: Could not open file '%s'\n", argv[1]);
        return 1;
    }

    printf("Lexical Analysis of: %s\n", argv[1]);
    printf("%-20s %-20s %-8s %s\n", "LEXEME", "TOKEN TYPE", "LINE", "COLUMN");

    Token token;
    do
    {
        token = get_next_token(fp);

        printf("%-20s %-20s %-8d %d\n",
               token.lexeme,
               token.type == TOKEN_EOF ? "EOF" : token.type == TOKEN_KEYWORD  ? "KEYWORD"
                                             : token.type == TOKEN_IDENTIFIER ? "IDENTIFIER"
                                             : token.type == TOKEN_LITERAL    ? "LITERAL"
                                             : token.type == TOKEN_OPERATOR   ? "OPERATOR"
                                             : token.type == TOKEN_SEPARATOR  ? "SEPARATOR"
                                             : token.type == TOKEN_ERROR      ? "ERROR"
                                                                              : "UNKNOWN",
               token.line,
               token.column);

    } while (token.type != TOKEN_EOF);

    fclose(fp);
    return 0;
}
