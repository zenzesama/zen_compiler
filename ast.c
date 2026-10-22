#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ast.h"

Node *make_number(int value) {
    Node *n = malloc(sizeof(Node));
    
    n->type = NODE_NUMBER;
    n->number = value;

    return n;
}

Node *make_binaryop(OpKind op, Node *left, Node *right) {
    Node *n = malloc(sizeof(Node));
    
    n->type = NODE_BINARYOP;
    n->op = op;
    n->left = left;
    n->right = right;

    return n;
}

Node *make_neg(Node *operand) {
    Node *n = malloc(sizeof(Node));

    n->type = NODE_NEG;
    n->operand = operand;
    return n;
}

Node *make_identifier(char *name) {
    Node *n = malloc(sizeof(Node));
    
    n->type = NODE_IDENTIFIER;
    strcpy(n->name, name);
    return n;
}

Node *make_var_decl(char *name, Node *init) {
    Node *n = malloc(sizeof(Node));

    n->type = NODE_VAR_DECL;
    strcpy(n->name, name);
    n->operand = init;
    return n;
}

Node *make_assign(char *name, Node *value) {
    Node *n = malloc(sizeof(Node));

    n->type = NODE_ASSIGN;
    strcpy(n->name, name);
    n->operand = value;
    return n;
}

Node *make_print(Node *operand) {
    Node *n = malloc(sizeof(Node));

    n->type = NODE_PRINT;
    n->operand = operand;
    return n;
}


Node *make_if(Node *condition, Node *then_branch, Node *else_branch) {
    Node *n = malloc(sizeof(Node));

    n->type = NODE_IF;
    n->condition = condition;
    n->then_branch = then_branch;
    n->else_branch = else_branch;

    return n;
}

Node *make_block() {
    Node *n = malloc(sizeof(Node));
    
    n->type = NODE_BLOCK;
    n->stmt_count = 0;

    return n;
}

void block_add(Node *block, Node *stmt) {
    if (block->stmt_count >= 256) {
        fprintf(stderr, "AST Error: Can't have more than 256 statements. \n");
        exit(1);
    }

    block->statements[block->stmt_count++] = stmt;
}

Node *make_not(Node *operand) {
    Node *n = malloc(sizeof(Node));

    n->type = NODE_NOT;
    n->operand = operand;
    return n;
}

void free_node(Node *node) {
    if (!node) 
        return;

    switch (node->type) {
        case NODE_NUMBER:
            break;
        case NODE_BINARYOP:
            free_node(node->left);
            free_node(node->right);
            break;
        case NODE_NEG:
            free_node(node->operand);
            break;
        case NODE_IDENTIFIER:
            break;
        case NODE_VAR_DECL:
            free_node(node->operand);
            break;
        case NODE_ASSIGN:
            free_node(node->operand);
            break;
        case NODE_PRINT:
            free_node(node->operand);
            break;
        case NODE_IF:
            free_node(node->condition);
            free_node(node->then_branch);
            free_node(node->else_branch);
            break;
        case NODE_BLOCK:
            {
            int n = node->stmt_count;
            while (n--) {
                free_node(node->statements[n]);
            }
            break;
            }
        case NODE_NOT:
            free_node(node->operand);
            break;
    }
    free(node);
}
