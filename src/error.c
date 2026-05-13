/*
 * Error Handler Implementation
 *
 * Source file for error handling.
 */

#include "../include/error.h"
#include <stdarg.h>
#include <stdio.h>

// Global error counters
int error_count = 0;
int warning_count = 0;
int syntax_error_count = 0;
int semantic_error_count = 0;

void reset_error_counters(void)
{
    error_count = 0;
    warning_count = 0;
    syntax_error_count = 0;
    semantic_error_count = 0;
}

ErrorStats get_error_stats(void)
{
    ErrorStats stats;
    stats.syntax_errors = syntax_error_count;
    stats.semantic_errors = semantic_error_count;
    stats.type_errors = 0;  // Can expand this later
    stats.total_errors = error_count;
    stats.warnings = warning_count;
    return stats;
}

void error_report(ErrorType type, int line, int column, const char *message)
{
    const char *error_type_str = "";
    switch (type)
    {
    case ERROR_SYNTAX:
        error_type_str = "Syntax Error";
        syntax_error_count++;
        break;
    case ERROR_SEMANTIC:
        error_type_str = "Semantic Error";
        semantic_error_count++;
        break;
    case ERROR_TYPE_MISMATCH:
        error_type_str = "Type Mismatch Error";
        semantic_error_count++;
        break;
    case ERROR_UNDEFINED_SYMBOL:
        error_type_str = "Undefined Symbol Error";
        semantic_error_count++;
        break;
    case ERROR_REDEFINED_SYMBOL:
        error_type_str = "Redefined Symbol Error";
        semantic_error_count++;
        break;
    case ERROR_INVALID_OPERATION:
        error_type_str = "Invalid Operation Error";
        semantic_error_count++;
        break;
    }

    fprintf(stderr, "[%s] Line %d, Column %d: %s\n", error_type_str, line, column, message);
    error_count++;
}

void error_print(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}
