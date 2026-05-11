/*
 * Semantic Analyzer Implementation
 *
 * Source file for semantic analysis.
 */

#include "../include/semantic.h"
#include "../include/error.h"
#include "../include/utils.h"
#include <stdio.h>
#include <string.h>

// Forward declarations
static int semantic_check_node(SemanticAnalyzer *analyzer, ASTNode *node);
static DataType semantic_infer_type(SemanticAnalyzer *analyzer, ASTNode *node);

SemanticAnalyzer *semantic_create(void)
{
    SemanticAnalyzer *analyzer = (SemanticAnalyzer *)xcalloc(1, sizeof(SemanticAnalyzer));
    analyzer->symtab = symtab_create();
    analyzer->error_count = 0;
    return analyzer;
}

void semantic_free(SemanticAnalyzer *analyzer)
{
    if (analyzer != NULL)
    {
        if (analyzer->symtab != NULL)
        {
            symtab_free(analyzer->symtab);
        }
        xfree(analyzer);
    }
}

int semantic_analyze(SemanticAnalyzer *analyzer, ASTNode *ast)
{
    if (analyzer == NULL || ast == NULL)
        return 0;

    semantic_check_node(analyzer, ast);
    return analyzer->error_count == 0;
}

static int semantic_check_node(SemanticAnalyzer *analyzer, ASTNode *node)
{
    if (node == NULL)
        return 1;

    switch (node->type)
    {
    case NODE_PROGRAM:
        symtab_enter_scope(analyzer->symtab);
        for (int i = 0; i < node->data.program.num_declarations; i++)
        {
            semantic_check_node(analyzer, node->data.program.declarations[i]);
        }
        symtab_exit_scope(analyzer->symtab);
        break;

    case NODE_FUNCTION_DECL:
        // Define function in symbol table
        if (!symtab_define(analyzer->symtab, node->data.function_decl.name,
                          SYMBOL_FUNCTION, node->data.function_decl.return_type, 0))
        {
            error_report(ERROR_REDEFINED_SYMBOL, node->line, node->column,
                        "Function already defined");
            analyzer->error_count++;
        }

        // Enter function scope
        symtab_enter_scope(analyzer->symtab);

        // Add parameters to symbol table
        for (int i = 0; i < node->data.function_decl.num_parameters; i++)
        {
            ASTNode *param = node->data.function_decl.parameters[i];
            if (param->type == NODE_VAR_DECL)
            {
                symtab_define(analyzer->symtab, param->data.var_decl.name,
                            SYMBOL_PARAM, param->data.var_decl.data_type,
                            param->data.var_decl.is_array);
            }
        }

        // Check function body
        semantic_check_node(analyzer, node->data.function_decl.body);

        symtab_exit_scope(analyzer->symtab);
        break;

    case NODE_VAR_DECL:
        // Define variable in symbol table
        if (!symtab_define(analyzer->symtab, node->data.var_decl.name,
                          SYMBOL_VAR, node->data.var_decl.data_type,
                          node->data.var_decl.is_array))
        {
            error_report(ERROR_REDEFINED_SYMBOL, node->line, node->column,
                        "Variable already defined");
            analyzer->error_count++;
        }

        // Check initializer
        if (node->data.var_decl.initializer != NULL)
        {
            semantic_check_node(analyzer, node->data.var_decl.initializer);
        }
        break;

    case NODE_BLOCK:
        symtab_enter_scope(analyzer->symtab);
        for (int i = 0; i < node->data.block.num_statements; i++)
        {
            semantic_check_node(analyzer, node->data.block.statements[i]);
        }
        symtab_exit_scope(analyzer->symtab);
        break;

    case NODE_EXPRESSION_STMT:
        semantic_check_node(analyzer, node->data.expression_stmt.expression);
        break;

    case NODE_IF_STMT:
        semantic_check_node(analyzer, node->data.if_stmt.condition);
        semantic_check_node(analyzer, node->data.if_stmt.then_branch);
        if (node->data.if_stmt.else_branch != NULL)
        {
            semantic_check_node(analyzer, node->data.if_stmt.else_branch);
        }
        break;

    case NODE_WHILE_STMT:
        semantic_check_node(analyzer, node->data.while_stmt.condition);
        semantic_check_node(analyzer, node->data.while_stmt.body);
        break;

    case NODE_FOR_STMT:
        symtab_enter_scope(analyzer->symtab);
        if (node->data.for_stmt.init != NULL)
        {
            semantic_check_node(analyzer, node->data.for_stmt.init);
        }
        if (node->data.for_stmt.condition != NULL)
        {
            semantic_check_node(analyzer, node->data.for_stmt.condition);
        }
        if (node->data.for_stmt.increment != NULL)
        {
            semantic_check_node(analyzer, node->data.for_stmt.increment);
        }
        semantic_check_node(analyzer, node->data.for_stmt.body);
        symtab_exit_scope(analyzer->symtab);
        break;

    case NODE_RETURN_STMT:
        if (node->data.return_stmt.value != NULL)
        {
            semantic_check_node(analyzer, node->data.return_stmt.value);
        }
        break;

    case NODE_ASSIGNMENT:
        semantic_check_node(analyzer, node->data.assignment.target);
        semantic_check_node(analyzer, node->data.assignment.value);
        break;

    case NODE_BINARY_OP:
        semantic_check_node(analyzer, node->data.binary_op.left);
        semantic_check_node(analyzer, node->data.binary_op.right);
        break;

    case NODE_UNARY_OP:
        semantic_check_node(analyzer, node->data.unary_op.operand);
        break;

    case NODE_FUNCTION_CALL:
        // Check if function is defined
        if (symtab_lookup(analyzer->symtab, node->data.function_call.function_name) == NULL)
        {
            error_report(ERROR_UNDEFINED_SYMBOL, node->line, node->column,
                        "Undefined function");
            analyzer->error_count++;
        }

        // Check arguments
        for (int i = 0; i < node->data.function_call.num_arguments; i++)
        {
            semantic_check_node(analyzer, node->data.function_call.arguments[i]);
        }
        break;

    case NODE_IDENTIFIER:
        // Check if identifier is defined
        if (symtab_lookup(analyzer->symtab, node->data.identifier.name) == NULL)
        {
            error_report(ERROR_UNDEFINED_SYMBOL, node->line, node->column,
                        "Undefined variable");
            analyzer->error_count++;
        }
        break;

    case NODE_ARRAY_ACCESS:
        // Check if array is defined
        if (symtab_lookup(analyzer->symtab, node->data.array_access.array_name) == NULL)
        {
            error_report(ERROR_UNDEFINED_SYMBOL, node->line, node->column,
                        "Undefined array");
            analyzer->error_count++;
        }
        semantic_check_node(analyzer, node->data.array_access.index);
        break;

    case NODE_CAST:
        semantic_check_node(analyzer, node->data.cast.expression);
        break;

    case NODE_LITERAL:
    case NODE_BREAK_STMT:
    case NODE_CONTINUE_STMT:
        // Nothing to check
        break;
    }

    return 1;
}

DataType semantic_get_type(SemanticAnalyzer *analyzer, ASTNode *node)
{
    if (node == NULL)
        return TYPE_UNKNOWN;

    return semantic_infer_type(analyzer, node);
}

static DataType semantic_infer_type(SemanticAnalyzer *analyzer, ASTNode *node)
{
    if (node == NULL)
        return TYPE_UNKNOWN;

    switch (node->type)
    {
    case NODE_LITERAL:
        switch (node->data.literal.literal_type)
        {
        case LITERAL_INT:
            return TYPE_INT;
        case LITERAL_FLOAT:
            return TYPE_FLOAT;
        case LITERAL_STRING:
            return TYPE_CHAR; // Simplified
        case LITERAL_CHAR:
            return TYPE_CHAR;
        }
        break;

    case NODE_IDENTIFIER:
    {
        Symbol *sym = symtab_lookup(analyzer->symtab, node->data.identifier.name);
        if (sym != NULL)
        {
            return sym->type;
        }
        return TYPE_UNKNOWN;
    }

    case NODE_BINARY_OP:
        // Simplified: assume result is int for now
        return TYPE_INT;

    case NODE_UNARY_OP:
        return semantic_infer_type(analyzer, node->data.unary_op.operand);

    case NODE_FUNCTION_CALL:
    {
        Symbol *sym = symtab_lookup(analyzer->symtab, node->data.function_call.function_name);
        if (sym != NULL && sym->kind == SYMBOL_FUNCTION)
        {
            return sym->type;
        }
        return TYPE_UNKNOWN;
    }

    case NODE_ARRAY_ACCESS:
        // Return element type
        {
            Symbol *sym = symtab_lookup(analyzer->symtab, node->data.array_access.array_name);
            if (sym != NULL)
            {
                return sym->type;
            }
        }
        break;

    case NODE_CAST:
        return node->data.cast.target_type;

    default:
        return TYPE_UNKNOWN;
    }

    return TYPE_UNKNOWN;
}
