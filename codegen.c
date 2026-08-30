#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"

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
    char *buff = malloc(32);

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

void codegen_start(FILE *out, Node *exp) {
    fprintf(out, "declare i32 @printf(ptr, ...)\n\n");

    fprintf(out, "@fmt = private constant [4 x i8] c\"%%d\\0A\\00\"\n\n");
    
    fprintf(out, "define i32 @main() {\n");
    fprintf(out, "entry:\n");

    char *result = emit(out, exp);

    fprintf(out, "call i32 (ptr, ...) @printf(ptr @fmt, i32 %s)\n", result);
    fprintf(out, "ret i32 0\n"); 
    fprintf(out, "}\n");

    free(result);
}
