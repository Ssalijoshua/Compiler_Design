/*
 * Error Handler
 *
 * Manages error reporting and recovery.
 * Provides consistent error messages with locations and context.
 */

#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>

typedef enum
{
    ERROR_SYNTAX,
    ERROR_SEMANTIC,
    ERROR_TYPE_MISMATCH,
    ERROR_UNDEFINED_SYMBOL,
    ERROR_REDEFINED_SYMBOL,
    ERROR_INVALID_OPERATION,
} ErrorType;

void error_report(ErrorType type, int line, int column, const char *message);
void error_print(const char *format, ...);

extern int error_count;
extern int warning_count;

#endif
