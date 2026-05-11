/*
 * Utility Functions Implementation
 *
 * Source file for common utility functions.
 */

#include "../include/utils.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

void *xmalloc(size_t size)
{
    void *ptr = malloc(size);
    if (ptr == NULL && size > 0)
    {
        fprintf(stderr, "Error: Memory allocation failed (%zu bytes)\n", size);
        exit(1);
    }
    return ptr;
}

void *xcalloc(size_t count, size_t size)
{
    void *ptr = calloc(count, size);
    if (ptr == NULL && count > 0 && size > 0)
    {
        fprintf(stderr, "Error: Memory allocation failed (%zu x %zu bytes)\n", count, size);
        exit(1);
    }
    return ptr;
}

void *xrealloc(void *ptr, size_t size)
{
    void *new_ptr = realloc(ptr, size);
    if (new_ptr == NULL && size > 0)
    {
        fprintf(stderr, "Error: Memory reallocation failed (%zu bytes)\n", size);
        exit(1);
    }
    return new_ptr;
}

void xfree(void *ptr)
{
    if (ptr != NULL)
    {
        free(ptr);
    }
}

char *xstrdup(const char *str)
{
    if (str == NULL)
        return NULL;

    size_t len = strlen(str) + 1;
    char *dup = (char *)xmalloc(len);
    strcpy(dup, str);
    return dup;
}

char *str_concat(const char *a, const char *b)
{
    if (a == NULL)
        a = "";
    if (b == NULL)
        b = "";

    size_t len = strlen(a) + strlen(b) + 1;
    char *result = (char *)xmalloc(len);
    strcpy(result, a);
    strcat(result, b);
    return result;
}

void debug_print(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}
