#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "ast.h"
#include "lexer.h"

static int temp_count = 0;

static const char *op_instr(char op) {
    switch (op) {
        case '+' : return "add";
        case '-' : return "sub";
        case '*' : return "mul";
        case '/' : return "sdiv";
        case '%' : return "srem";
        default : return "add";
    }
}

static char *emit(FILE *out, Node *node) {
    char *buff = malloc(128);

    if (node->type == NODE_VAR_DECL) {
        sprintf(buff, "%%%s = alloca i32\n", node->name);
        if (node->operand) {
            char *init = emit(out, node->operand);
            sprintf(buff + strlen(buff),
                    "store i32 %s, ptr %%%s\n", init, node->name);
            free(init);
        }
        return buff;
    }

    if (node->type == NODE_IDENTIFIER) {
        int id = ++temp_count;
        sprintf(buff, "%%%d = load i32, ptr %%%s\n", id, node->name);
        return buff;
    }

    if (node->type == NODE_ASSIGN) {
        char *rhs = emit(out, node->operand);
        sprintf(buff, "store i32 %s, ptr %%%s\n", rhs, node->name);
        free(rhs);
        return buff;
    }

    if (node->type == NODE_NUMBER) {
        sprintf(buff, "%d", node->number);
        return buff;
    }

    if (node->type == NODE_NEG) {
        char *inner = emit(out, node->operand);
        int id = ++temp_count;
        fprintf(out, "%%%d = sub i32 0, %s\n", id, inner);
        free(inner);
        sprintf(buff, "%%%d", id);
        return buff;
    }
    
    if (node->type == NODE_BINARYOP) {
        char *left = emit(out, node->left);
        char *right = emit(out, node->right);
        int id = ++temp_count;
        fprintf(out, "%%%d = %s i32 %s, %s\n", id, op_instr(node->op), left, right);
        free(left);
        free(right);
        sprintf(buff, "%%%d", id);
        return buff;
    }
    
    fprintf(stderr, "code gen error. Bad node type.\n");
    return buff;
}

void codegen_start(FILE *out, Program *prgm) {
    fprintf(out, "declare i32 @printf(ptr, ...)\n\n");

    fprintf(out, "@fmt = private constant [4 x i8] c\"%%d\\0A\\00\"\n\n");
    
    fprintf(out, "define i32 @main() {\n");
    fprintf(out, "entry:\n");

    for (int i = 0; i < prgm->count; i++) {
        char *result = emit(out, prgm->statements[i]);
        fprintf(out, "%s", result);
        free(result);
    }

    fprintf(out, "ret i32 0\n"); 
    fprintf(out, "}\n");
}
