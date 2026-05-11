/*
 * Intermediate Code Implementation
 *
 * Source file for IR generation and manipulation.
 */

#include "../include/ir.h"
#include "../include/utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void ir_generate_node(IRGenerator *ir, ASTNode *node, char *result);

IRGenerator *ir_create(void)
{
    IRGenerator *ir = (IRGenerator *)xcalloc(1, sizeof(IRGenerator));
    ir->num_instructions = 0;
    ir->next_label = 0;
    ir->next_temp = 0;
    return ir;
}

void ir_free(IRGenerator *ir)
{
    if (ir != NULL)
    {
        xfree(ir);
    }
}

void ir_emit(IRGenerator *ir, IROpType op, const char *op1, const char *op2, const char *result)
{
    if (ir == NULL || ir->num_instructions >= MAX_IR_INSTRUCTIONS)
        return;

    IRInstruction *instr = &ir->instructions[ir->num_instructions];
    instr->op = op;

    if (op1)
    {
        strncpy(instr->op1.name, op1, sizeof(instr->op1.name) - 1);
        instr->op1.name[sizeof(instr->op1.name) - 1] = '\0';
    }
    else
    {
        instr->op1.name[0] = '\0';
    }

    if (op2)
    {
        strncpy(instr->op2.name, op2, sizeof(instr->op2.name) - 1);
        instr->op2.name[sizeof(instr->op2.name) - 1] = '\0';
    }
    else
    {
        instr->op2.name[0] = '\0';
    }

    if (result)
    {
        strncpy(instr->result.name, result, sizeof(instr->result.name) - 1);
        instr->result.name[sizeof(instr->result.name) - 1] = '\0';
    }
    else
    {
        instr->result.name[0] = '\0';
    }

    ir->num_instructions++;
}

void ir_emit_label(IRGenerator *ir, const char *label)
{
    ir_emit(ir, IR_LABEL, label, NULL, NULL);
}

void ir_emit_jump(IRGenerator *ir, const char *label)
{
    ir_emit(ir, IR_JUMP, label, NULL, NULL);
}

void ir_emit_jump_if_zero(IRGenerator *ir, const char *cond, const char *label)
{
    ir_emit(ir, IR_JUMP_IF_ZERO, cond, label, NULL);
}

char *ir_new_temp(IRGenerator *ir)
{
    static char temp_name[256];
    if (ir->next_temp < MAX_TEMP_VARS)
    {
        sprintf(temp_name, "t%d", ir->next_temp);
        ir->next_temp++;
    }
    return temp_name;
}

char *ir_new_label(IRGenerator *ir)
{
    static char label_name[256];
    sprintf(label_name, "L%d", ir->next_label);
    ir->next_label++;
    return label_name;
}

void ir_print(IRGenerator *ir)
{
    if (ir == NULL)
        return;

    printf("\n=== INTERMEDIATE CODE (THREE-ADDRESS CODE) ===\n");

    for (int i = 0; i < ir->num_instructions; i++)
    {
        IRInstruction *instr = &ir->instructions[i];

        printf("%4d: ", i);

        switch (instr->op)
        {
        case IR_ASSIGN:
            printf("%s = %s", instr->result.name, instr->op1.name);
            break;
        case IR_ADD:
            printf("%s = %s + %s", instr->result.name, instr->op1.name, instr->op2.name);
            break;
        case IR_SUB:
            printf("%s = %s - %s", instr->result.name, instr->op1.name, instr->op2.name);
            break;
        case IR_MUL:
            printf("%s = %s * %s", instr->result.name, instr->op1.name, instr->op2.name);
            break;
        case IR_DIV:
            printf("%s = %s / %s", instr->result.name, instr->op1.name, instr->op2.name);
            break;
        case IR_MOD:
            printf("%s = %s %% %s", instr->result.name, instr->op1.name, instr->op2.name);
            break;
        case IR_EQ:
            printf("%s = %s == %s", instr->result.name, instr->op1.name, instr->op2.name);
            break;
        case IR_NE:
            printf("%s = %s != %s", instr->result.name, instr->op1.name, instr->op2.name);
            break;
        case IR_LT:
            printf("%s = %s < %s", instr->result.name, instr->op1.name, instr->op2.name);
            break;
        case IR_LE:
            printf("%s = %s <= %s", instr->result.name, instr->op1.name, instr->op2.name);
            break;
        case IR_GT:
            printf("%s = %s > %s", instr->result.name, instr->op1.name, instr->op2.name);
            break;
        case IR_GE:
            printf("%s = %s >= %s", instr->result.name, instr->op1.name, instr->op2.name);
            break;
        case IR_LABEL:
            printf("%s:", instr->op1.name);
            break;
        case IR_JUMP:
            printf("goto %s", instr->op1.name);
            break;
        case IR_JUMP_IF_ZERO:
            printf("if %s == 0 goto %s", instr->op1.name, instr->op2.name);
            break;
        case IR_JUMP_IF_NOT_ZERO:
            printf("if %s != 0 goto %s", instr->op1.name, instr->op2.name);
            break;
        case IR_CALL:
            printf("%s = call %s", instr->result.name, instr->op1.name);
            break;
        case IR_RETURN:
            if (instr->op1.name[0] != '\0')
                printf("return %s", instr->op1.name);
            else
                printf("return");
            break;
        case IR_PARAM:
            printf("param %s", instr->op1.name);
            break;
        }

        printf("\n");
    }
}

int ir_generate(IRGenerator *ir, ASTNode *ast)
{
    if (ir == NULL || ast == NULL)
        return 0;

    ir_generate_node(ir, ast, NULL);
    return 1;
}

static void ir_generate_node(IRGenerator *ir, ASTNode *node, char *result)
{
    if (node == NULL)
        return;

    switch (node->type)
    {
    case NODE_PROGRAM:
        for (int i = 0; i < node->data.program.num_declarations; i++)
        {
            ir_generate_node(ir, node->data.program.declarations[i], NULL);
        }
        break;

    case NODE_FUNCTION_DECL:
        // Emit function label
        ir_emit_label(ir, node->data.function_decl.name);
        ir_generate_node(ir, node->data.function_decl.body, NULL);
        break;

    case NODE_BLOCK:
        for (int i = 0; i < node->data.block.num_statements; i++)
        {
            ir_generate_node(ir, node->data.block.statements[i], NULL);
        }
        break;

    case NODE_VAR_DECL:
        if (node->data.var_decl.initializer != NULL)
        {
            char temp[256];
            strcpy(temp, node->data.var_decl.name);
            ir_generate_node(ir, node->data.var_decl.initializer, temp);
        }
        break;

    case NODE_EXPRESSION_STMT:
        ir_generate_node(ir, node->data.expression_stmt.expression, NULL);
        break;

    case NODE_ASSIGNMENT:
    {
        char temp[256];
        if (node->data.assignment.target->type == NODE_IDENTIFIER)
        {
            strcpy(temp, node->data.assignment.target->data.identifier.name);
        }
        else
        {
            strcpy(temp, ir_new_temp(ir));
        }
        ir_generate_node(ir, node->data.assignment.value, temp);
        break;
    }

    case NODE_RETURN_STMT:
        if (node->data.return_stmt.value != NULL)
        {
            char temp[256];
            strcpy(temp, ir_new_temp(ir));
            ir_generate_node(ir, node->data.return_stmt.value, temp);
            ir_emit(ir, IR_RETURN, temp, NULL, NULL);
        }
        else
        {
            ir_emit(ir, IR_RETURN, NULL, NULL, NULL);
        }
        break;

    case NODE_IF_STMT:
    {
        char *else_label = ir_new_label(ir);
        char *end_label = ir_new_label(ir);
        char temp[256];
        strcpy(temp, ir_new_temp(ir));

        ir_generate_node(ir, node->data.if_stmt.condition, temp);
        ir_emit_jump_if_zero(ir, temp, else_label);

        ir_generate_node(ir, node->data.if_stmt.then_branch, NULL);

        if (node->data.if_stmt.else_branch != NULL)
        {
            ir_emit_jump(ir, end_label);
            ir_emit_label(ir, else_label);
            ir_generate_node(ir, node->data.if_stmt.else_branch, NULL);
            ir_emit_label(ir, end_label);
        }
        else
        {
            ir_emit_label(ir, else_label);
        }
        break;
    }

    case NODE_WHILE_STMT:
    {
        char *loop_label = ir_new_label(ir);
        char *end_label = ir_new_label(ir);
        char temp[256];

        ir_emit_label(ir, loop_label);
        strcpy(temp, ir_new_temp(ir));
        ir_generate_node(ir, node->data.while_stmt.condition, temp);
        ir_emit_jump_if_zero(ir, temp, end_label);
        ir_generate_node(ir, node->data.while_stmt.body, NULL);
        ir_emit_jump(ir, loop_label);
        ir_emit_label(ir, end_label);
        break;
    }

    case NODE_BINARY_OP:
    {
        if (result == NULL)
            result = ir_new_temp(ir);

        char left_temp[256], right_temp[256];
        strcpy(left_temp, ir_new_temp(ir));
        strcpy(right_temp, ir_new_temp(ir));

        ir_generate_node(ir, node->data.binary_op.left, left_temp);
        ir_generate_node(ir, node->data.binary_op.right, right_temp);

        if (strcmp(node->data.binary_op.operator, "+") == 0)
            ir_emit(ir, IR_ADD, left_temp, right_temp, result);
        else if (strcmp(node->data.binary_op.operator, "-") == 0)
            ir_emit(ir, IR_SUB, left_temp, right_temp, result);
        else if (strcmp(node->data.binary_op.operator, "*") == 0)
            ir_emit(ir, IR_MUL, left_temp, right_temp, result);
        else if (strcmp(node->data.binary_op.operator, "/") == 0)
            ir_emit(ir, IR_DIV, left_temp, right_temp, result);
        else if (strcmp(node->data.binary_op.operator, "%") == 0)
            ir_emit(ir, IR_MOD, left_temp, right_temp, result);
        else if (strcmp(node->data.binary_op.operator, "==") == 0)
            ir_emit(ir, IR_EQ, left_temp, right_temp, result);
        else if (strcmp(node->data.binary_op.operator, "!=") == 0)
            ir_emit(ir, IR_NE, left_temp, right_temp, result);
        else if (strcmp(node->data.binary_op.operator, "<") == 0)
            ir_emit(ir, IR_LT, left_temp, right_temp, result);
        else if (strcmp(node->data.binary_op.operator, "<=") == 0)
            ir_emit(ir, IR_LE, left_temp, right_temp, result);
        else if (strcmp(node->data.binary_op.operator, ">") == 0)
            ir_emit(ir, IR_GT, left_temp, right_temp, result);
        else if (strcmp(node->data.binary_op.operator, ">=") == 0)
            ir_emit(ir, IR_GE, left_temp, right_temp, result);
        break;
    }

    case NODE_LITERAL:
        if (result != NULL)
        {
            ir_emit(ir, IR_ASSIGN, node->data.literal.value, NULL, result);
        }
        break;

    case NODE_IDENTIFIER:
        if (result != NULL)
        {
            ir_emit(ir, IR_ASSIGN, node->data.identifier.name, NULL, result);
        }
        break;

    default:
        break;
    }
}
