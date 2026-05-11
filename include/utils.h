/*
 * Utility Functions
 *
 * Common utility functions used across the compiler.
 * Memory management, string operations, and data structures.
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

// Memory wrappers
void *xmalloc(size_t size);
void *xcalloc(size_t count, size_t size);
void *xrealloc(void *ptr, size_t size);
void xfree(void *ptr);

// String utilities
char *xstrdup(const char *str);
char *str_concat(const char *a, const char *b);

// Debug utilities
void debug_print(const char *format, ...);

#endif
