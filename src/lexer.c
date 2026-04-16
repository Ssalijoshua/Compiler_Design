#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "../include/lexer.h"

static int line = 1;
static int column = 1;

// Helper function to check if a lexeme is a keyword
static int is_keyword(const char *lexeme)
{
    const char *keywords[] = {
        "int", "return", "if", "else",
        "void", "char", "float", "double",
        "while", "for", "do", "break", "continue",
        "struct", "typedef", "switch", "case", "default",
        NULL};

    for (int i = 0; keywords[i] != NULL; i++)
    {
        if (strcmp(lexeme, keywords[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

Token get_next_token(FILE *fp)
{
    int c;

    // Skip whitespace and comments
    while (1)
    {
        c = fgetc(fp);

        if (isspace(c))
        {
            if (c == '\n')
            {
                line++;
                column = 1;
            }
            else
            {
                column++;
            }
            continue;
        }

        // Handle single-line comments
        if (c == '/')
        {
            int next = fgetc(fp);
            if (next == '/')
            {
                // Skip until end of line
                while ((c = fgetc(fp)) != '\n' && c != EOF)
                {
                    column++;
                }
                if (c == '\n')
                {
                    line++;
                    column = 1;
                }
                continue;
            }
            // Handle block comments
            else if (next == '*')
            {
                int prev = 0;
                while ((c = fgetc(fp)) != EOF)
                {
                    if (prev == '*' && c == '/')
                    {
                        column++;
                        break;
                    }
                    if (c == '\n')
                    {
                        line++;
                        column = 1;
                    }
                    else
                    {
                        column++;
                    }
                    prev = c;
                }
                continue;
            }
            // Not a comment, put the character back
            ungetc(next, fp);
            break;
        }
        break;
    }

    Token token;
    token.line = line;
    token.column = column;

    // Handle EOF
    if (c == EOF)
    {
        token.type = TOKEN_EOF;
        token.lexeme[0] = '\0';
        return token;
    }

    // Identify Words (Keywords or Identifiers)
    if (isalpha(c) || c == '_')
    {
        int i = 0;
        while (isalnum(c) || c == '_')
        {
            token.lexeme[i++] = c;
            c = fgetc(fp);
            column++;
        }
        ungetc(c, fp);
        column--;
        token.lexeme[i] = '\0';

        token.type = is_keyword(token.lexeme) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;
        return token;
    }

    // Identify Numbers (integers and floats)
    if (isdigit(c))
    {
        int i = 0;
        while (isdigit(c))
        {
            token.lexeme[i++] = c;
            c = fgetc(fp);
            column++;
        }

        // Check for decimal point
        if (c == '.')
        {
            int next = fgetc(fp);
            if (isdigit(next))
            {
                token.lexeme[i++] = '.';
                column++;
                c = next;
                while (isdigit(c))
                {
                    token.lexeme[i++] = c;
                    c = fgetc(fp);
                    column++;
                }
                ungetc(c, fp);
                column--;
            }
            else
            {
                ungetc(next, fp);
                ungetc(c, fp);
                column--;
            }
        }
        else
        {
            ungetc(c, fp);
            column--;
        }

        token.lexeme[i] = '\0';
        token.type = TOKEN_LITERAL;
        return token;
    }

    // Handle string literals
    if (c == '"')
    {
        int i = 0;
        token.lexeme[i++] = c;
        column++;
        while ((c = fgetc(fp)) != '"' && c != EOF && c != '\n')
        {
            token.lexeme[i++] = c;
            if (c == '\\' && i < 255)
            {
                token.lexeme[i++] = fgetc(fp); // Handle escape sequences
            }
            column++;
        }
        if (c == '"')
        {
            token.lexeme[i++] = c;
            column++;
        }
        token.lexeme[i] = '\0';
        token.type = TOKEN_LITERAL;
        return token;
    }

    // Handle character literals
    if (c == '\'')
    {
        int i = 0;
        token.lexeme[i++] = c;
        column++;
        while ((c = fgetc(fp)) != '\'' && c != EOF && c != '\n')
        {
            token.lexeme[i++] = c;
            if (c == '\\' && i < 255)
            {
                token.lexeme[i++] = fgetc(fp); // Handle escape sequences
            }
            column++;
        }
        if (c == '\'')
        {
            token.lexeme[i++] = c;
            column++;
        }
        token.lexeme[i] = '\0';
        token.type = TOKEN_LITERAL;
        return token;
    }

    // Handle Operators (including multi-char operators)
    int next_c;

    if (c == '+')
    {
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        token.type = TOKEN_OPERATOR;
        column++;
        return token;
    }
    if (c == '-')
    {
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        token.type = TOKEN_OPERATOR;
        column++;
        return token;
    }
    if (c == '*')
    {
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        token.type = TOKEN_OPERATOR;
        column++;
        return token;
    }
    if (c == '%')
    {
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        token.type = TOKEN_OPERATOR;
        column++;
        return token;
    }
    if (c == '=')
    {
        token.lexeme[0] = c;
        next_c = fgetc(fp);
        if (next_c == '=')
        {
            token.lexeme[1] = next_c;
            token.lexeme[2] = '\0';
            column += 2;
        }
        else
        {
            ungetc(next_c, fp);
            token.lexeme[1] = '\0';
            column++;
        }
        token.type = TOKEN_OPERATOR;
        return token;
    }
    if (c == '!')
    {
        next_c = fgetc(fp);
        if (next_c == '=')
        {
            strcpy(token.lexeme, "!=");
            token.type = TOKEN_OPERATOR;
            column += 2;
            return token;
        }
        ungetc(next_c, fp);
        token.type = TOKEN_ERROR;
        token.lexeme[0] = '!';
        token.lexeme[1] = '\0';
        column++;
        return token;
    }
    if (c == '<')
    {
        token.lexeme[0] = c;
        next_c = fgetc(fp);
        if (next_c == '=')
        {
            token.lexeme[1] = next_c;
            token.lexeme[2] = '\0';
            column += 2;
        }
        else
        {
            ungetc(next_c, fp);
            token.lexeme[1] = '\0';
            column++;
        }
        token.type = TOKEN_OPERATOR;
        return token;
    }
    if (c == '>')
    {
        token.lexeme[0] = c;
        next_c = fgetc(fp);
        if (next_c == '=')
        {
            token.lexeme[1] = next_c;
            token.lexeme[2] = '\0';
            column += 2;
        }
        else
        {
            ungetc(next_c, fp);
            token.lexeme[1] = '\0';
            column++;
        }
        token.type = TOKEN_OPERATOR;
        return token;
    }
    if (c == '&')
    {
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        token.type = TOKEN_OPERATOR;
        column++;
        return token;
    }
    if (c == '|')
    {
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        token.type = TOKEN_OPERATOR;
        column++;
        return token;
    }

    // Handle Separators
    switch (c)
    {
    case '{':
    case '}':
    case '(':
    case ')':
    case '[':
    case ']':
    case ';':
    case ',':
    case '.':
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        token.type = TOKEN_SEPARATOR;
        column++;
        return token;
    }

    // Fallback for errors
    token.lexeme[0] = c;
    token.lexeme[1] = '\0';
    token.type = TOKEN_ERROR;
    column++;
    return token;
}