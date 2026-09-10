#include <stdlib.h>
#include <string.h>
#include "ast.h"

Node *make_number(int value) {
    Node *n = malloc(sizeof(Node));
    
    n->type = NODE_NUMBER;
    n->number = value;

    return n;
}

Node *make_binaryop(char op, Node *left, Node *right) {
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
    }
    free(node);
}
