/*
 * Symbol Table Manager Implementation
 *
 * Source file for symbol table management.
 */

#include "../include/symtab.h"
#include "../include/utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

SymbolTable *symtab_create(void)
{
    SymbolTable *table = (SymbolTable *)xcalloc(1, sizeof(SymbolTable));
    table->num_symbols = 0;
    table->scope_level = 0;
    return table;
}

void symtab_free(SymbolTable *table)
{
    if (table != NULL)
    {
        xfree(table);
    }
}

void symtab_enter_scope(SymbolTable *table)
{
    if (table != NULL)
    {
        table->scope_level++;
    }
}

void symtab_exit_scope(SymbolTable *table)
{
    if (table != NULL && table->scope_level > 0)
    {
        // Don't remove symbols - just decrement scope level
        // This preserves all symbols for the final symbol table output
        // (In a real compiler, you'd want to manage scope properly, but for
        //  display purposes, we keep everything)
        table->scope_level--;
    }
}

int symtab_define(SymbolTable *table, const char *name, SymbolKind kind, DataType type, int is_array)
{
    if (table == NULL || name == NULL)
        return 0;

    // Check if symbol already exists in current scope
    for (int i = 0; i < table->num_symbols; i++)
    {
        if (table->symbols[i].scope_level == table->scope_level &&
            strcmp(table->symbols[i].name, name) == 0)
        {
            return 0; // Already defined in this scope
        }
    }

    // Check if we have space
    if (table->num_symbols >= MAX_SYMBOLS)
        return 0;

    // Add new symbol
    Symbol *sym = &table->symbols[table->num_symbols];
    strncpy(sym->name, name, sizeof(sym->name) - 1);
    sym->name[sizeof(sym->name) - 1] = '\0';
    sym->kind = kind;
    sym->type = type;
    sym->is_array = is_array;
    sym->is_initialized = 0;
    sym->scope_level = table->scope_level;

    table->num_symbols++;
    return 1;
}

Symbol *symtab_lookup(SymbolTable *table, const char *name)
{
    if (table == NULL || name == NULL)
        return NULL;

    // Search from current scope up to global scope
    for (int i = table->num_symbols - 1; i >= 0; i--)
    {
        if (strcmp(table->symbols[i].name, name) == 0)
        {
            return &table->symbols[i];
        }
    }

    return NULL;
}

Symbol *symtab_lookup_local(SymbolTable *table, const char *name)
{
    if (table == NULL || name == NULL)
        return NULL;

    // Search only in current scope
    for (int i = table->num_symbols - 1; i >= 0; i--)
    {
        if (table->symbols[i].scope_level == table->scope_level &&
            strcmp(table->symbols[i].name, name) == 0)
        {
            return &table->symbols[i];
        }
    }

    return NULL;
}

void symtab_print(SymbolTable *table)
{
    if (table == NULL || table->num_symbols == 0)
    {
        printf("Symbol Table: (empty)\n");
        return;
    }

    printf("\n=== SYMBOL TABLE ===\n");
    printf("%-25s %-12s %-10s %-8s %s\n", "NAME", "KIND", "TYPE", "ARRAY", "SCOPE");
    printf("%-25s %-12s %-10s %-8s %s\n", "----", "----", "----", "-----", "-----");

    for (int i = 0; i < table->num_symbols; i++)
    {
        Symbol *sym = &table->symbols[i];

        const char *kind_str = sym->kind == SYMBOL_VAR ? "Variable" : sym->kind == SYMBOL_FUNCTION ? "Function"
                                                                  : sym->kind == SYMBOL_PARAM      ? "Parameter"
                                                                                                   : "Unknown";

        const char *type_str = sym->type == TYPE_INT ? "int" : sym->type == TYPE_FLOAT ? "float"
                                                           : sym->type == TYPE_CHAR    ? "char"
                                                           : sym->type == TYPE_VOID    ? "void"
                                                                                       : "unknown";

        const char *array_str = sym->is_array ? "yes" : "no";

        printf("%-25s %-12s %-10s %-8s %d\n",
               sym->name, kind_str, type_str, array_str, sym->scope_level);
    }
    printf("\n");
}
