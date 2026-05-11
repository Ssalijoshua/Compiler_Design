/*
 * Intermediate Code (IR) Definition
 *
 * Defines the intermediate representation (IR) of the program.
 * This serves as a bridge between AST and machine code generation.
 * Uses three-address code (TAC) representation.
 */

#ifndef IR_H
#define IR_H

#include "ast.h"

#define MAX_IR_INSTRUCTIONS 10000
#define MAX_TEMP_VARS 1000

typedef enum
{
    IR_ASSIGN,
    IR_ADD,
    IR_SUB,
    IR_MUL,
    IR_DIV,
    IR_MOD,
    IR_EQ,
    IR_NE,
    IR_LT,
    IR_LE,
    IR_GT,
    IR_GE,
    IR_LABEL,
    IR_JUMP,
    IR_JUMP_IF_ZERO,
    IR_JUMP_IF_NOT_ZERO,
    IR_CALL,
    IR_RETURN,
    IR_PARAM,
} IROpType;

typedef struct
{
    char name[256];
} IROperand;

typedef struct
{
    IROpType op;
    IROperand op1;
    IROperand op2;
    IROperand result;
} IRInstruction;

typedef struct
{
    IRInstruction instructions[MAX_IR_INSTRUCTIONS];
    int num_instructions;
    int next_label;
    int next_temp;
} IRGenerator;

IRGenerator *ir_create(void);
void ir_free(IRGenerator *ir);
void ir_emit(IRGenerator *ir, IROpType op, const char *op1, const char *op2, const char *result);
void ir_emit_label(IRGenerator *ir, const char *label);
void ir_emit_jump(IRGenerator *ir, const char *label);
void ir_emit_jump_if_zero(IRGenerator *ir, const char *cond, const char *label);
char *ir_new_temp(IRGenerator *ir);
char *ir_new_label(IRGenerator *ir);
void ir_print(IRGenerator *ir);
int ir_generate(IRGenerator *ir, ASTNode *ast);

#endif
