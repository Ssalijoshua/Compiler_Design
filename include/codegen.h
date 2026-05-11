/*
 * Code Generator
 *
 * Converts intermediate representation (IR) to assembly code.
 * Generates assembly code output.
 */

#ifndef CODEGEN_H
#define CODEGEN_H

#include "ir.h"
#include <stdio.h>

typedef struct
{
    FILE *output;
    int label_count;
} CodeGenerator;

CodeGenerator *codegen_create(FILE *output);
void codegen_free(CodeGenerator *gen);
int codegen_generate(CodeGenerator *gen, IRGenerator *ir);

#endif
