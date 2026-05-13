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

    if (ast == NULL)
    {
        fprintf(stderr, "Error: Failed to parse program\n");
        parser_free(parser);
        return 1;
    }

    if (mode == 1)
    {
        printf("=== ABSTRACT SYNTAX TREE ===\n");
        ast_print(ast, 0);
        printf("\n");
        ast_free(ast);
        parser_free(parser);
        return 0;
    }

    // ===== PHASE 2: SEMANTIC ANALYSIS =====
    printf("\n=== PHASE 2: SEMANTIC ANALYSIS ===\n");

    SemanticAnalyzer *analyzer = semantic_create();
    if (analyzer == NULL)
    {
        fprintf(stderr, "Error: Failed to create semantic analyzer\n");
        ast_free(ast);
        parser_free(parser);
        return 1;
    }

    int semantic_ok = semantic_analyze(analyzer, ast);
    printf("Semantic analysis complete.\n");
    printf("Errors: %d\n\n", error_count);

    if (!semantic_ok)
    {
        fprintf(stderr, "Warning: Semantic errors found, continuing anyway...\n");
    }

    if (mode == 2)
    {
        printf("=== ABSTRACT SYNTAX TREE ===\n");
        ast_print(ast, 0);
        printf("\n");
        semantic_free(analyzer);
        ast_free(ast);
        parser_free(parser);
        return error_count > 0 ? 1 : 0;
    }

    // ===== PHASE 3: INTERMEDIATE CODE GENERATION =====
    printf("\n=== PHASE 3: INTERMEDIATE CODE GENERATION ===\n");

    IRGenerator *ir = ir_create();
    if (ir == NULL)
    {
        fprintf(stderr, "Error: Failed to create IR generator\n");
        semantic_free(analyzer);
        ast_free(ast);
        parser_free(parser);
        return 1;
    }

    ir_generate(ir, ast);
    ir_print(ir);

    if (mode == 3)
    {
        printf("\n");
        ir_free(ir);
        semantic_free(analyzer);
        ast_free(ast);
        parser_free(parser);
        return 0;
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

    CodeGenerator *codegen = codegen_create(out_fp);
    if (codegen == NULL)
    {
        fprintf(stderr, "Error: Failed to create code generator\n");
        ir_free(ir);
        semantic_free(analyzer);
        ast_free(ast);
        parser_free(parser);
        return 1;
    }

    codegen_generate(codegen, ir);

    if (out_fp != stdout)
    {
        fclose(out_fp);
    }

    printf("\n=== COMPILATION SUCCESSFUL ===\n");
    printf("Phases completed: Lexer -> Parser -> Semantic Analysis -> IR -> Codegen\n");
    printf("Errors: %d | Warnings: %d\n", error_count, warning_count);

    // Cleanup
    codegen_free(codegen);
    ir_free(ir);
    semantic_free(analyzer);
    ast_free(ast);
    parser_free(parser);

    return error_count > 0 ? 1 : 0;
}
