#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "ast.h"
#include "lexer.h"

static int temp_count = 0;
static char declared[128][64];
static int declared_count = 0;

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

static int already_declared(const char *name) {
    for (int i = 0; i < declared_count; i++) {
        if (strcmp(declared[i], name) == 0) return 1;
    }
    return 0;
}

static void declare_name(const char *name) {
    if (already_declared(name)) {
        fprintf(stderr, "Code gen error:\"%s\" redeclared.\n", name);
        exit(1);
    }
    if (declared_count >= 128) {
        fprintf(stderr, "Code gen error: Too many variables, cap is 128.\n");
    }
    strcpy(declared[declared_count++], name);
}

static void require_declared(const char *name) {
    if (!already_declared(name)) {
        fprintf(stderr, "Code gen error: \"%s\" undeclared.\n", name);
        exit(1);
    }
}

static char *emit(FILE *out, Node *node) {
    char *buff = malloc(128);

    if (node->type == NODE_PRINT) {
        char *val = emit(out, node->operand);
        fprintf(out, "call i32 (ptr, ...) @printf(ptr @fmt, i32 %s)\n", val);
        free(val);
        return buff;
    }

    if (node->type == NODE_VAR_DECL) {
        declare_name(node->name);
        fprintf(out, "%%%s = alloca i32\n", node->name);
        if (node->operand) {
            char *init = emit(out, node->operand);
            fprintf(out, "store i32 %s, ptr %%%s\n", init, node->name);
            free(init);
        }
        return buff;
    }

    if (node->type == NODE_IDENTIFIER) {
        require_declared(node->name);
        int id = ++temp_count;
        fprintf(out, "%%%d = load i32, ptr %%%s\n", id, node->name);
        sprintf(buff, "%%%d", id);
        return buff;
    }

    if (node->type == NODE_ASSIGN) {
        require_declared(node->name);
        char *rhs = emit(out, node->operand);
        fprintf(out, "store i32 %s, ptr %%%s\n", rhs, node->name);
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
    
    fprintf(stderr, "Code gen error: Bad node type.\n");
    return buff;
}

void codegen_start(FILE *out, Program *prgm) {
    fprintf(out, "declare i32 @printf(ptr, ...)\n\n");

    fprintf(out, "@fmt = private constant [4 x i8] c\"%%d\\0A\\00\"\n\n");
    
    fprintf(out, "define i32 @main() {\n");
    fprintf(out, "entry:\n");

    for (int i = 0; i < prgm->count; i++) {
        char *result = emit(out, prgm->statements[i]);
        free(result);
    }

    fprintf(out, "ret i32 0\n"); 
    fprintf(out, "}\n");
}
