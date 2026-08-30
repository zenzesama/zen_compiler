#ifndef AST_H
#define AST_H

typedef enum {
    NODE_NUMBER, 
    NODE_BINARYOP, 
    NODE_NEG
} NodeType;

typedef struct Node {
    NodeType type;
    int number;
    char op;
    struct Node *left;
    struct Node *right;
    struct Node *operand;
} Node;

Node *make_number(int);
Node *make_binaryop(char, Node *, Node *);
Node *make_neg(Node *);
void free_node(Node *);

#endif
