/*
 * Semantic Analyzer
 *
 * Performs semantic analysis on the AST.
 * Tasks include type checking, symbol resolution, and scope verification.
 */

#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include "symtab.h"

typedef struct
{
    SymbolTable *symtab;
    int error_count;
} SemanticAnalyzer;

SemanticAnalyzer *semantic_create(void);
void semantic_free(SemanticAnalyzer *analyzer);
int semantic_analyze(SemanticAnalyzer *analyzer, ASTNode *ast);
DataType semantic_get_type(SemanticAnalyzer *analyzer, ASTNode *node);

#endif
