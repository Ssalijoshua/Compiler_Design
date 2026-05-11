/*
 * Code Generator Implementation
 *
 * Source file for assembly code generation.
 */

#include "../include/codegen.h"
#include "../include/utils.h"
#include <stdlib.h>
#include <string.h>

CodeGenerator *codegen_create(FILE *output)
{
    CodeGenerator *gen = (CodeGenerator *)xcalloc(1, sizeof(CodeGenerator));
    gen->output = output;
    gen->label_count = 0;
    return gen;
}

void codegen_free(CodeGenerator *gen)
{
    if (gen != NULL)
    {
        xfree(gen);
    }
}

int codegen_generate(CodeGenerator *gen, IRGenerator *ir)
{
    if (gen == NULL || ir == NULL)
        return 0;

    fprintf(gen->output, "\n=== GENERATED CODE (ASSEMBLY-LIKE) ===\n");
    fprintf(gen->output, ".section .text\n");
    fprintf(gen->output, ".global main\n\n");

    // Generate code for each IR instruction
    for (int i = 0; i < ir->num_instructions; i++)
    {
        IRInstruction *instr = &ir->instructions[i];

        switch (instr->op)
        {
        case IR_LABEL:
            fprintf(gen->output, "%s:\n", instr->op1.name);
            break;

        case IR_ASSIGN:
            fprintf(gen->output, "  mov %%%s, %%%s\n", instr->op1.name, instr->result.name);
            break;

        case IR_ADD:
            fprintf(gen->output, "  mov %%%s, %%rax\n", instr->op1.name);
            fprintf(gen->output, "  add %%%s, %%rax\n", instr->op2.name);
            fprintf(gen->output, "  mov %%rax, %%%s\n", instr->result.name);
            break;

        case IR_SUB:
            fprintf(gen->output, "  mov %%%s, %%rax\n", instr->op1.name);
            fprintf(gen->output, "  sub %%%s, %%rax\n", instr->op2.name);
            fprintf(gen->output, "  mov %%rax, %%%s\n", instr->result.name);
            break;

        case IR_MUL:
            fprintf(gen->output, "  mov %%%s, %%rax\n", instr->op1.name);
            fprintf(gen->output, "  imul %%%s, %%rax\n", instr->op2.name);
            fprintf(gen->output, "  mov %%rax, %%%s\n", instr->result.name);
            break;

        case IR_DIV:
            fprintf(gen->output, "  mov %%%s, %%rax\n", instr->op1.name);
            fprintf(gen->output, "  cqo\n");
            fprintf(gen->output, "  idiv %%%s\n", instr->op2.name);
            fprintf(gen->output, "  mov %%rax, %%%s\n", instr->result.name);
            break;

        case IR_MOD:
            fprintf(gen->output, "  mov %%%s, %%rax\n", instr->op1.name);
            fprintf(gen->output, "  cqo\n");
            fprintf(gen->output, "  idiv %%%s\n", instr->op2.name);
            fprintf(gen->output, "  mov %%rdx, %%%s\n", instr->result.name);
            break;

        case IR_EQ:
            fprintf(gen->output, "  cmp %%%s, %%%s\n", instr->op2.name, instr->op1.name);
            fprintf(gen->output, "  sete %%al\n");
            fprintf(gen->output, "  movzx %%al, %%%s\n", instr->result.name);
            break;

        case IR_NE:
            fprintf(gen->output, "  cmp %%%s, %%%s\n", instr->op2.name, instr->op1.name);
            fprintf(gen->output, "  setne %%al\n");
            fprintf(gen->output, "  movzx %%al, %%%s\n", instr->result.name);
            break;

        case IR_LT:
            fprintf(gen->output, "  cmp %%%s, %%%s\n", instr->op2.name, instr->op1.name);
            fprintf(gen->output, "  setl %%al\n");
            fprintf(gen->output, "  movzx %%al, %%%s\n", instr->result.name);
            break;

        case IR_LE:
            fprintf(gen->output, "  cmp %%%s, %%%s\n", instr->op2.name, instr->op1.name);
            fprintf(gen->output, "  setle %%al\n");
            fprintf(gen->output, "  movzx %%al, %%%s\n", instr->result.name);
            break;

        case IR_GT:
            fprintf(gen->output, "  cmp %%%s, %%%s\n", instr->op2.name, instr->op1.name);
            fprintf(gen->output, "  setg %%al\n");
            fprintf(gen->output, "  movzx %%al, %%%s\n", instr->result.name);
            break;

        case IR_GE:
            fprintf(gen->output, "  cmp %%%s, %%%s\n", instr->op2.name, instr->op1.name);
            fprintf(gen->output, "  setge %%al\n");
            fprintf(gen->output, "  movzx %%al, %%%s\n", instr->result.name);
            break;

        case IR_JUMP:
            fprintf(gen->output, "  jmp %s\n", instr->op1.name);
            break;

        case IR_JUMP_IF_ZERO:
            fprintf(gen->output, "  cmp $0, %%%s\n", instr->op1.name);
            fprintf(gen->output, "  je %s\n", instr->op2.name);
            break;

        case IR_JUMP_IF_NOT_ZERO:
            fprintf(gen->output, "  cmp $0, %%%s\n", instr->op1.name);
            fprintf(gen->output, "  jne %s\n", instr->op2.name);
            break;

        case IR_RETURN:
            if (instr->op1.name[0] != '\0')
            {
                fprintf(gen->output, "  mov %%%s, %%rax\n", instr->op1.name);
            }
            fprintf(gen->output, "  ret\n");
            break;

        case IR_PARAM:
            fprintf(gen->output, "  push %%%s\n", instr->op1.name);
            break;

        case IR_CALL:
            fprintf(gen->output, "  call %s\n", instr->op1.name);
            fprintf(gen->output, "  mov %%rax, %%%s\n", instr->result.name);
            break;
        }
    }

    fprintf(gen->output, "\n.end\n");
    return 1;
}
