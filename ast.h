#ifndef AST_H
#define AST_H

typedef enum {
    NODE_NUMBER, 
    NODE_BINARYOP, 
    NODE_NEG, 
    NODE_IDENTIFIER, 
    NODE_VAR_DECL, 
    NODE_ASSIGN, 
    NODE_PRINT
} NodeType;

typedef struct Node {
    NodeType type;
    int number;
    char op;
    char name[64];
    struct Node *left;
    struct Node *right;
    struct Node *operand;
} Node;

typedef struct {
    Node *statements[256];
    int count;
} Program;

Node *make_number(int value);
Node *make_binaryop(char op, Node *left, Node *right);
Node *make_neg(Node *operand);
Node *make_identifier(char *name);
Node *make_var_decl(char *name, Node *init);
Node *make_assign(char *name, Node *value);
Node *make_print(Node *operand);
void free_node(Node *node);

#endif
