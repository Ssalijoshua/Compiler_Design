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

// Error tracking structures
typedef struct
{
    int syntax_errors;
    int semantic_errors;
    int type_errors;
    int total_errors;
    int warnings;
} ErrorStats;

void error_report(ErrorType type, int line, int column, const char *message);
void error_print(const char *format, ...);
void reset_error_counters(void);
ErrorStats get_error_stats(void);

extern int error_count;
extern int warning_count;
extern int syntax_error_count;
extern int semantic_error_count;

#endif
