#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "ast.h"
#include "lexer.h"

static int temp_count = 0;
static char declared[128][64];
static int declared_count = 0;
static int label_count = 0;
static char funcs[64][64];
static int func_params[64];
static int func_count = 0;

static char *emit(FILE *out, Node *node);

static const char *op_instr(OpKind op) {
    switch (op) {
        case OP_ADD : return "add";
        case OP_SUB : return "sub";
        case OP_MUL : return "mul";
        case OP_DIV : return "sdiv";
        case OP_MOD : return "srem";
        default :
              fprintf(stderr, "Codegen error: Invalid operator.\n");
              exit(1);
    }
}

static const char *cmp_instr(OpKind op) {
    switch (op) {
        case OP_EQ:  return "eq";
        case OP_NEQ: return "ne";
        case OP_LT:  return "slt";
        case OP_LTE: return "sle";
        case OP_GT:  return "sgt";
        case OP_GTE: return "sge";
        default:     return NULL;
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
        exit(1);
    }
    strcpy(declared[declared_count++], name);
}

static void require_declared(const char *name) {
    if (!already_declared(name)) {
        fprintf(stderr, "Code gen error: \"%s\" undeclared.\n", name);
        exit(1);
    }
}

static int find_func(const char *name) {
    for (int i = 0; i < func_count; i++) {
        if (strcmp(funcs[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

static void register_func(Node *f) {
    if (strcmp(f->name, "main") == 0 || 
        strcmp(f->name, "printf") == 0 || 
        strcmp(f->name, "fmt") == 0) {
        fprintf(stderr, "Code gen error: \"%s\" redeclared.\n", f->name);
        exit(1);
    }

    if (find_func(f->name) != -1) {
        fprintf(stderr, "Code gen error: Function \"%s\" redeclared.\n", f->name);
        exit(1);
    }

    if (func_count >= 64) {
        fprintf(stderr, "Code gen error: Too many functions, cap is 64.\n");
        exit(1);
    }

    strcpy(funcs[func_count], f->name);
    func_params[func_count] = f->parm_count;
    func_count++;
}

static void hoist_allocas(FILE *out, Node *node) {
    if (!node) return;

    switch (node->type) {
        case NODE_VAR_DECL:
            fprintf(out, "%%%s = alloca i32\n", node->name);
            break;
        case NODE_BLOCK:
            for (int i = 0; i < node->stmt_count; i++) {
                hoist_allocas(out, node->statements[i]);
            }
            break;
        case NODE_IF:
            hoist_allocas(out, node->then_branch);
            hoist_allocas(out, node->else_branch);
            break;
        case NODE_WHILE:
            hoist_allocas(out, node->body);
            break;
        default:
            break;
    }
}

static void emit_function(FILE *out, Node *f) {
    temp_count = 0;
    declared_count = 0;

    fprintf(out, "define i32 @%s(", f->name);

    for (int i = 0; i < f->parm_count; i++) {
        if (i > 0) {
            fprintf(out, ", ");
        }
        fprintf(out, "i32 %%%s.arg", f->parms[i]);
    }

    fprintf(out, ") {\n");
    fprintf(out, "entry:\n");

    for (int i = 0; i < f->parm_count; i++) {
        declare_name(f->parms[i]);
        fprintf(out, "%%%s = alloca i32\n", f->parms[i]);
        fprintf(out, "store i32 %%%s.arg, ptr %%%s\n", f->parms[i], f->parms[i]);
    }

    hoist_allocas(out, f->body);
    char *result = emit(out, f->body);
    free(result);
    fprintf(out, "ret i32 0\n");
    fprintf(out, "}\n\n");
}

static char *emit(FILE *out, Node *node) {
    char *buff = malloc(128);

    if (node->type == NODE_RETURN) {
        char *val = emit(out, node->operand);
        fprintf(out, "ret i32 %s\n", val);
        free(val);
        int id = ++label_count;
        fprintf(out, "ret.after.%d:\n", id);
        return buff;
    }

    if (node->type == NODE_PRINT) {
        char *val = emit(out, node->operand);
        int id = ++temp_count;
        fprintf(out, "%%%d = call i32 (ptr, ...) @printf(ptr @fmt, i32 %s)\n", id, val);
        free(val);
        return buff;
    }

    if (node->type == NODE_VAR_DECL) {
        declare_name(node->name);
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
        const char *cmp = cmp_instr(node->op);
        if (cmp) {
            int cmp_id = ++temp_count;
            fprintf(out, "%%%d = icmp %s i32 %s, %s\n", cmp_id, cmp, left, right);
            free(left);
            free(right);
            int id = ++temp_count;
            fprintf(out, "%%%d = zext i1 %%%d to i32\n", id, cmp_id);
            sprintf(buff, "%%%d", id);
            return buff;
        } 
        if (node->op == OP_AND || node->op == OP_OR) {
            char *operator = node->op == OP_AND ? "and" : "or";
            int l = ++temp_count;
            fprintf(out, "%%%d = icmp ne i32 %s, 0\n", l, left);
            int r = ++temp_count;
            fprintf(out, "%%%d = icmp ne i32 %s, 0\n", r, right);
            free(left);
            free(right);
            int logic = ++temp_count;
            fprintf(out, "%%%d = %s i1 %%%d, %%%d\n", 
                    logic, operator, l, r);
            int id = ++temp_count;
            fprintf(out, "%%%d = zext i1 %%%d to i32\n", id, logic);
            sprintf(buff, "%%%d", id);
            return buff;
        }
        int id = ++temp_count;
        fprintf(out, "%%%d = %s i32 %s, %s\n", id, op_instr(node->op), left, right);
        free(left);
        free(right);
        sprintf(buff, "%%%d", id);
        return buff;
    }

    if (node->type == NODE_NOT) {
        char *inner = emit(out, node->operand);
        int cmp_id = ++temp_count;
        fprintf(out, "%%%d = icmp eq i32 %s, 0\n", cmp_id, inner);
        free(inner);
        int id = ++temp_count;
        fprintf(out, "%%%d = zext i1 %%%d to i32\n", id, cmp_id);
        sprintf(buff, "%%%d", id);
        return buff;
    }
    

    if (node->type == NODE_BLOCK) {
        for (int i = 0; i < node->stmt_count; i++) {
            char *result = emit(out, node->statements[i]);
            free(result);
        }
        return buff;
    }

    if (node->type == NODE_IF) {
        int id = ++label_count;
        char *cond = emit(out, node->condition);
        int cmp = ++temp_count;
        fprintf(out, "%%%d = icmp ne i32 %s, 0\n", cmp, cond);
        free(cond);
        if (node->else_branch) {
            fprintf(out, "br i1 %%%d, label %%if.then.%d, label %%if.else.%d\n", 
                    cmp, id, id);
        }
        else {
            fprintf(out, "br i1 %%%d, label %%if.then.%d, label %%if.end.%d\n", 
                    cmp, id, id);
        }
        fprintf(out, "if.then.%d:\n", id);
        char *then_result = emit(out, node->then_branch);
        free(then_result);
        fprintf(out, "br label %%if.end.%d\n", id);
        if (node->else_branch) {
            fprintf(out, "if.else.%d:\n", id);
            char *else_result = emit(out, node->else_branch);
            free(else_result);
            fprintf(out, "br label %%if.end.%d\n", id);
        }
        fprintf(out, "if.end.%d:\n", id);
        return buff;
    }

    if (node->type == NODE_WHILE) {
        int id = ++label_count;
        fprintf(out, "br label %%while.cond.%d\n", id);
        fprintf(out, "while.cond.%d:\n", id);
        char *cond = emit(out, node->condition);
        int cmp = ++temp_count;
        fprintf(out, "%%%d = icmp ne i32 %s, 0\n", cmp, cond);
        fprintf(out, "br i1 %%%d, label %%while.body.%d, label %%while.end.%d\n", cmp, id, id);
        fprintf(out, "while.body.%d:\n", id);
        char *body_result = emit(out, node->body);
        free(body_result);
        fprintf(out, "br label %%while.cond.%d\n", id);
        fprintf(out, "while.end.%d:\n", id);
        free(cond);
        return buff;
    }

    if (node->type == NODE_CALL) {
        int idx = find_func(node->name);
        if (idx == -1) {
            fprintf(stderr, "Code gen error: Unidentified function \"%s\".\n", node->name);
            exit(1);
        }
        if (node->arg_count != func_params[idx]) {
            fprintf(stderr, "Code gen error: Function \"%s\" expects %d arguments, for %d.\n", 
                    node->name, 
                    func_params[idx], 
                    node->arg_count);
            exit(1);
        }
        char *vals[8];
        if (node->arg_count > 8) {
            fprintf(stderr, "Code gen error: Maximum 8 function arguments allowed.\n");
            exit(1);
        }
        for (int i = 0; i < node->arg_count; i++) {
            vals[i] = emit(out, node->args[i]);
        }
        int id = ++temp_count;
        fprintf(out, "%%%d = call i32 @%s(", id, node->name);
        for (int i = 0; i < node->arg_count; i++) {
            if (i > 0) {
                fprintf(out, ", ");
            }
            fprintf(out, "i32 %s", vals[i]);
            free(vals[i]);
        }
        fprintf(out, ")\n");
        sprintf(buff, "%%%d", id);
        return buff;
    }

    fprintf(stderr, "Code gen error: Bad node type.\n");
    exit(1);
    return buff;
}

void codegen_start(FILE *out, Program *prgm) {
    fprintf(out, "declare i32 @printf(ptr, ...)\n\n");

    fprintf(out, "@fmt = private constant [4 x i8] c\"%%d\\0A\\00\"\n\n");

    for (int i = 0; i < prgm->count; i++) {
        Node *node = prgm->statements[i];
        if (node->type == NODE_FUNC) {
            register_func(node);
        }
    }

    for (int i = 0; i < prgm->count; i++) {
        Node *node = prgm->statements[i];
        if (node->type == NODE_FUNC) {
            emit_function(out, node);
        }
    }

    temp_count = 0;
    declared_count = 0;

    fprintf(out, "define i32 @main() {\n");
    fprintf(out, "entry:\n");

    for (int i = 0; i < prgm->count; i++) {
        Node *node = prgm->statements[i];
        if (node->type != NODE_FUNC) {
            hoist_allocas(out, node);
        }
    }

    for (int i = 0; i < prgm->count; i++) {
        Node *node = prgm->statements[i];
        if (node->type != NODE_FUNC) {
            char *result = emit(out, node);
            free(result);
        }
    }

    fprintf(out, "ret i32 0\n"); 
    fprintf(out, "}\n");
}
