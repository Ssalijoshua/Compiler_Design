#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/ast.h"
#include "../include/semantic.h"
#include "../include/ir.h"
#include "../include/codegen.h"
#include "../include/error.h"

void print_usage(const char *prog)
{
    printf("Usage: %s [options] <source_file>\n", prog);
    printf("\nOptions:\n");
    printf("  -lexer       : Run lexical analysis only\n");
    printf("  -parser      : Run parsing and build AST\n");
    printf("  -semantic    : Run semantic analysis\n");
    printf("  -ir          : Generate intermediate code\n");
    printf("  -codegen     : Generate assembly code (default)\n");
    printf("\nExample:\n");
    printf("  %s examples/test_lexer.c\n", prog);
    printf("  %s -parser examples/test_lexer.c\n", prog);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        print_usage(argv[0]);
        return 1;
    }

    // Parse command line arguments
    int mode = 5; // 5 = full compilation (codegen)
    const char *filename = NULL;

    if (argc >= 3)
    {
        if (strcmp(argv[1], "-lexer") == 0)
        {
            mode = 0;
            filename = argv[2];
        }
        else if (strcmp(argv[1], "-parser") == 0)
        {
            mode = 1;
            filename = argv[2];
        }
        else if (strcmp(argv[1], "-semantic") == 0)
        {
            mode = 2;
            filename = argv[2];
        }
        else if (strcmp(argv[1], "-ir") == 0)
        {
            mode = 3;
            filename = argv[2];
        }
        else if (strcmp(argv[1], "-codegen") == 0)
        {
            mode = 4;
            filename = argv[2];
        }
        else
        {
            filename = argv[1];
            mode = 5; // Full compilation
        }
    }
    else
    {
        filename = argv[1];
        mode = 5; // Full compilation
    }

    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Error: Could not open file '%s'\n", filename);
        return 1;
    }

    // ===== PHASE 0: LEXICAL ANALYSIS =====
    printf("=== PHASE 0: LEXICAL ANALYSIS ===\n");
    printf("File: %s\n", filename);
    printf("%-20s %-20s %-8s %s\n", "LEXEME", "TOKEN TYPE", "LINE", "COLUMN");
    printf("%-20s %-20s %-8s %s\n", "------", "----------", "----", "------");

    Token token;
    do
    {
        token = get_next_token(fp);

        printf("%-20s %-20s %-8d %d\n",
               token.lexeme,
               token.type == TOKEN_EOF ? "EOF" : token.type == TOKEN_KEYWORD  ? "KEYWORD"
                                             : token.type == TOKEN_IDENTIFIER ? "IDENTIFIER"
                                             : token.type == TOKEN_LITERAL    ? "LITERAL"
                                             : token.type == TOKEN_OPERATOR   ? "OPERATOR"
                                             : token.type == TOKEN_SEPARATOR  ? "SEPARATOR"
                                             : token.type == TOKEN_ERROR      ? "ERROR"
                                                                              : "UNKNOWN",
               token.line,
               token.column);

    } while (token.type != TOKEN_EOF);

    // If -lexer mode, stop here
    if (mode == 0)
    {
        printf("\n");
        fclose(fp);
        return 0;
    }

    // Rewind file for parser to use
    rewind(fp);
    reset_lexer(); // Reset lexer state for parsing

    // ===== PHASE 1: PARSING =====
    printf("=== PHASE 1: SYNTAX ANALYSIS & AST CONSTRUCTION ===\n");
    printf("File: %s\n", filename);

    Parser *parser = parser_create(fp);
    if (parser == NULL)
    {
        fprintf(stderr, "Error: Failed to create parser\n");
        fclose(fp);
        return 1;
    }

    ASTNode *ast = parser_parse(parser);

    printf("Errors: %d | Warnings: %d\n\n", parser->error_count, parser->warning_count);

    // Continue compilation even if there are syntax errors
    // This allows us to collect all errors (syntax and semantic) at once

    // If mode is 1 (parser only), don't continue further
    if (mode == 1)
    {
        printf("=== ABSTRACT SYNTAX TREE ===\n");
        if (ast != NULL)
        {
            ast_print(ast, 0);
        }
        else
        {
            printf("(AST could not be constructed due to syntax errors)\n");
        }
        printf("\n");
        if (ast != NULL)
        {
            ast_free(ast);
        }
        parser_free(parser);
        return parser->error_count > 0 ? 1 : 0;
    }

    // ===== PHASE 2: SEMANTIC ANALYSIS =====
    printf("\n=== PHASE 2: SEMANTIC ANALYSIS ===\n");

    SemanticAnalyzer *analyzer = semantic_create();
    if (analyzer == NULL)
    {
        fprintf(stderr, "Error: Failed to create semantic analyzer\n");
        if (ast != NULL)
        {
            ast_free(ast);
        }
        parser_free(parser);
        return 1;
    }

    reset_error_counters();

    // Only run semantic analysis if AST was successfully built
    if (ast != NULL)
    {
        int semantic_ok = semantic_analyze(analyzer, ast);
        printf("Semantic analysis complete.\n");
        printf("Errors: %d\n", semantic_error_count);

        // Print symbol table
        symtab_print(analyzer->symtab);

        if (!semantic_ok && semantic_error_count > 0)
        {
            fprintf(stderr, "Warning: Semantic errors found, continuing anyway...\n");
        }
    }
    else
    {
        printf("Semantic analysis skipped due to syntax errors in parsing.\n");
        printf("Errors: 0\n\n");
    }

    if (mode == 2)
    {
        printf("=== ABSTRACT SYNTAX TREE ===\n");
        if (ast != NULL)
        {
            ast_print(ast, 0);
        }
        else
        {
            printf("(AST could not be constructed due to syntax errors)\n");
        }
        printf("\n");
        semantic_free(analyzer);
        if (ast != NULL)
        {
            ast_free(ast);
        }
        parser_free(parser);
        return (parser->error_count + semantic_error_count) > 0 ? 1 : 0;
    }

    // ===== PHASE 3: INTERMEDIATE CODE GENERATION =====
    printf("\n=== PHASE 3: INTERMEDIATE CODE GENERATION ===\n");

    IRGenerator *ir = NULL;

    if (ast != NULL)
    {
        ir = ir_create();
        if (ir == NULL)
        {
            fprintf(stderr, "Error: Failed to create IR generator\n");
            semantic_free(analyzer);
            if (ast != NULL)
            {
                ast_free(ast);
            }
            parser_free(parser);
            return 1;
        }

        ir_generate(ir, ast);
        ir_print(ir);
    }
    else
    {
        printf("IR generation skipped due to syntax errors.\n");
    }

    if (mode == 3)
    {
        printf("\n");
        if (ir != NULL)
        {
            ir_free(ir);
        }
        semantic_free(analyzer);
        if (ast != NULL)
        {
            ast_free(ast);
        }
        parser_free(parser);
        return (parser->error_count + semantic_error_count) > 0 ? 1 : 0;
    }

    // ===== PHASE 4-5: CODE GENERATION =====
    printf("\n=== PHASE 4: CODE GENERATION ===\n");

    // Generate output file name in output folder
    char output_file[512];
    const char *base_filename = strrchr(filename, '/');
    if (base_filename == NULL)
    {
        base_filename = filename; // No path separator, use filename as is
    }
    else
    {
        base_filename++; // Skip the '/' character
    }

    sprintf(output_file, "output/%s", base_filename);
    // Replace .c with .s
    char *dot = strrchr(output_file, '.');
    if (dot != NULL)
    {
        strcpy(dot, ".s");
    }
    else
    {
        strcat(output_file, ".s");
    }

    FILE *out_fp = fopen(output_file, "w");
    if (out_fp == NULL)
    {
        // If we can't write to file, use stdout
        out_fp = stdout;
        printf("(Output to stdout)\n");
    }
    else
    {
        printf("Output file: %s\n", output_file);
    }

    if (ir != NULL)
    {
        CodeGenerator *codegen = codegen_create(out_fp);
        if (codegen == NULL)
        {
            fprintf(stderr, "Error: Failed to create code generator\n");
            if (out_fp != stdout)
            {
                fclose(out_fp);
            }
            ir_free(ir);
            semantic_free(analyzer);
            if (ast != NULL)
            {
                ast_free(ast);
            }
            parser_free(parser);
            return 1;
        }

        codegen_generate(codegen, ir);
        codegen_free(codegen);
    }
    else
    {
        printf("Code generation skipped due to errors in earlier phases.\n");
    }

    if (out_fp != stdout)
    {
        fclose(out_fp);
    }

    // Print compilation summary
    printf("\n========================================\n");
    printf("=== COMPILATION SUMMARY ===\n");
    printf("========================================\n");

    if (parser->error_count == 0 && semantic_error_count == 0)
    {
        printf("Phases completed: Lexer -> Parser -> Semantic Analysis -> IR -> Codegen\n");
    }
    else if (parser->error_count > 0)
    {
        printf("Compilation stopped after parsing due to syntax errors.\n");
    }
    else if (semantic_error_count > 0)
    {
        printf("Compilation completed with semantic errors.\n");
    }

    printf("\n--- Error Report ---\n");
    printf("Syntax Errors:     %d\n", parser->error_count);
    printf("Semantic Errors:   %d\n", semantic_error_count);
    printf("Total Errors:      %d\n", parser->error_count + semantic_error_count);
    printf("Warnings:          %d\n", parser->warning_count);
    printf("========================================\n");

    // Print symbol table at the end
    if (analyzer != NULL)
    {
        symtab_print(analyzer->symtab);
    }
    printf("========================================\n");

    if ((parser->error_count + semantic_error_count) == 0)
    {
        printf("=== COMPILATION SUCCESSFUL ===\n");
    }
    else
    {
        printf("=== COMPILATION COMPLETED WITH ERRORS ===\n");
    }
    printf("========================================\n");

    // Cleanup
    if (ir != NULL)
    {
        ir_free(ir);
    }
    semantic_free(analyzer);
    if (ast != NULL)
    {
        ast_free(ast);
    }
    parser_free(parser);

    return (parser->error_count + semantic_error_count) > 0 ? 1 : 0;
}
