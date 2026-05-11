/*
 * Error Handler Implementation
 *
 * Source file for error handling.
 */

#include "../include/error.h"
#include <stdarg.h>
#include <stdio.h>

int error_count = 0;
int warning_count = 0;

void error_report(ErrorType type, int line, int column, const char *message)
{
    const char *error_type_str = "";
    switch (type)
    {
    case ERROR_SYNTAX:
        error_type_str = "Syntax Error";
        break;
    case ERROR_SEMANTIC:
        error_type_str = "Semantic Error";
        break;
    case ERROR_TYPE_MISMATCH:
        error_type_str = "Type Mismatch";
        break;
    case ERROR_UNDEFINED_SYMBOL:
        error_type_str = "Undefined Symbol";
        break;
    case ERROR_REDEFINED_SYMBOL:
        error_type_str = "Redefined Symbol";
        break;
    case ERROR_INVALID_OPERATION:
        error_type_str = "Invalid Operation";
        break;
    }

    fprintf(stderr, "%s at line %d, column %d: %s\n", error_type_str, line, column, message);
    error_count++;
}

void error_print(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}
