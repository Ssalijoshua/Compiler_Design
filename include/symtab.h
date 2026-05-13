
#ifndef SYMTAB_H
#define SYMTAB_H

#include "ast.h"

#define MAX_SYMBOLS 1000
#define MAX_SCOPES 50

typedef enum
{
    SYMBOL_VAR,
    SYMBOL_FUNCTION,
    SYMBOL_PARAM,
} SymbolKind;

typedef struct
{
    char name[256];
    SymbolKind kind;
    DataType type;
    int is_array;
    int is_initialized;
    int scope_level;
} Symbol;

typedef struct
{
    Symbol symbols[MAX_SYMBOLS];
    int num_symbols;
    int scope_level;
} SymbolTable;

SymbolTable *symtab_create(void);
void symtab_free(SymbolTable *table);
void symtab_enter_scope(SymbolTable *table);
void symtab_exit_scope(SymbolTable *table);
int symtab_define(SymbolTable *table, const char *name, SymbolKind kind, DataType type, int is_array);
Symbol *symtab_lookup(SymbolTable *table, const char *name);
Symbol *symtab_lookup_local(SymbolTable *table, const char *name);
void symtab_print(SymbolTable *table);

#endif
