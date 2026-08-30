#include <stdlib.h>
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

void free_node(Node *node) {
    if (!node) 
        return;

    switch (node->type) {
        case NODE_NUMBER:
            break;
        case NODE_BINARYOP:
            free(node->left);
            free(node->right);
            break;
        case NODE_NEG:
            free(node->operand);
            break;
    }
    free(node);
}
