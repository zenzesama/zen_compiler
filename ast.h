#ifndef AST_H
#define AST_H

typedef enum {
    NODE_NUMBER, 
    NODE_BINARYOP, 
    NODE_NEG, 
    NODE_IDENTIFIER, 
    NODE_VAR_DECL, 
    NODE_ASSIGN, 
    NODE_PRINT, 
    NODE_IF, 
    NODE_BLOCK, 
    NODE_NOT, 
    NODE_WHILE, 
    NODE_FUNC, 
    NODE_RETURN, 
    NODE_CALL
} NodeType;

typedef enum {
    OP_ADD, 
    OP_SUB, 
    OP_MUL, 
    OP_DIV, 
    OP_MOD, 
    OP_EQ, 
    OP_NEQ, 
    OP_LT, 
    OP_LTE, 
    OP_GT, 
    OP_GTE, 
    OP_AND, 
    OP_OR
} OpKind;

typedef struct Node {
    NodeType type;
    int number;
    OpKind op;
    char name[64];
    struct Node *left;
    struct Node *right;
    struct Node *operand;
    struct Node *condition;
    struct Node *then_branch;
    struct Node *else_branch;
    int stmt_count;
    struct Node *statements[256];
    struct Node *body;
    char parms[8][64];
    int parm_count;
    struct Node *args[8];
    int arg_count;
} Node;

typedef struct {
    Node *statements[256];
    int count;
} Program;

Node *make_number(int value);
Node *make_binaryop(OpKind op, Node *left, Node *right);
Node *make_neg(Node *operand);
Node *make_identifier(char *name);
Node *make_var_decl(char *name, Node *init);
Node *make_assign(char *name, Node *value);
Node *make_print(Node *operand);
Node *make_if(Node *condition, Node *then_branch, Node *else_branch);
Node *make_block();
void block_add(Node *block, Node *stmt);
Node *make_not(Node *operand);
Node *make_while(Node *condition, Node *body);
Node *make_func(char *name);
void func_add_parm(Node *f, char *pname);
Node *make_return(Node *operand);
Node *make_call(char *name);
void call_add_arg(Node *call, Node *arg);
void free_node(Node *node);

#endif
