#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

typedef enum
{
    TOKEN_KEYWORD,    // int, return, if, else
    TOKEN_IDENTIFIER, // variable names, function names
    TOKEN_LITERAL,    // 10, 3.14, "hello"
    TOKEN_OPERATOR,   // +, -, *, /, =
    TOKEN_SEPARATOR,  // {, }, (, ), ;, ,
    TOKEN_EOF,        // End of file
    TOKEN_ERROR       // For invalid characters
} TokenType;

typedef struct
{
    TokenType type;
    char lexeme[256]; // The actual string (e.g., "int")
    int line;
    int column;
} Token;

// Function declarations
Token get_next_token(FILE *fp);

#endif
