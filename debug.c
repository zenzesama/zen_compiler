#include <stdio.h>
#include "debug.h"

static const char *token_type_name(TokenType type) {
    switch (type) {
        case TOKEN_NUMBER:              return "NUMBER";
        case TOKEN_PLUS:                return "PLUS";
        case TOKEN_MINUS:               return "MINUS";
        case TOKEN_STAR:                return "STAR";
        case TOKEN_SLASH:               return "SLASH";
        case TOKEN_PERCENT:             return "PERCENT";
        case TOKEN_LPAREN:              return "LPAREN";
        case TOKEN_RPAREN:              return "RPAREN";
        case TOKEN_SEMICOLON:           return "SEMICOLON";
        case TOKEN_EOF:                 return "EOF";
        case TOKEN_ASSIGN:              return "ASSIGN";
        case TOKEN_IDENTIFIER:          return "IDENTIFIER";
        case TOKEN_VAR:                 return "VAR";
        case TOKEN_INT:                 return "INT";
        case TOKEN_PRINT:               return "PRINT";
        case TOKEN_IF:                  return "IF";
        case TOKEN_ELSE:                return "ELSE";
        case TOKEN_EQUAL:               return "EQUAL";
        case TOKEN_NOT_EQUAL:           return "NOT EQUAL";
        case TOKEN_LESS_THAN:           return "LESS THAN";
        case TOKEN_LESS_THAN_EQUAL:     return "LESS THAN EQUAL";
        case TOKEN_GREATER_THAN:        return "GREATER THAN";
        case TOKEN_GREATER_THAN_EQUAL:  return "GREATER THAN EQUAL";
        case TOKEN_LOGIC_NOT:           return "LOGIC NOT";
        case TOKEN_LOGIC_AND:           return "LOGIC AND";
        case TOKEN_AND:                 return "AND";
        case TOKEN_LOGIC_OR:            return "LOGIC OR";
        case TOKEN_OR:                  return "OR";
        case TOKEN_LBRACE:              return "LBRACE";
        case TOKEN_RBRACE:              return "RBRACE";
        default:                        return "UNKNOWN";
    }
}

static const char *op_name(OpKind op) {
    switch (op) {
        case OP_ADD: return "+";
        case OP_SUB: return "-";
        case OP_MUL: return "*";
        case OP_DIV: return "/";
        case OP_MOD: return "%";
        case OP_EQ:  return "==";
        case OP_NEQ: return "!=";
        case OP_LT:  return "<";
        case OP_LTE: return "<=";
        case OP_GT:  return ">";
        case OP_GTE: return ">=";
        case OP_AND: return "&&";
        case OP_OR:  return "||";
        default:     return "UNKNOWN";
    }
}

static void print_token(Token t) {
    printf("%-12s", token_type_name(t.type));
    if (t.type == TOKEN_NUMBER) printf(" value = %d", t.value);
    if (t.type == TOKEN_IDENTIFIER) printf(" text = \"%s\"", t.text);
    printf("\n");
}

void lexer_test(Lexer *lex) {
    while (1) {
        Token t = lexer_next(lex);
        print_token(t);
        if (t.type == TOKEN_EOF) break;
    }
}

static void ast_test(Node *node, int depth) {    
    if (!node) return;

    for (int i = 0; i < depth; i++) printf("  ");

    switch (node->type) {
        case NODE_NUMBER:
            printf("NUMBER %d\n", node->number);
            break;
        case NODE_BINARYOP:
            printf("BINARY OP '%s'\n", op_name(node->op));
            ast_test(node->left, depth + 1);
            ast_test(node->right, depth + 1);
            break;
        case NODE_NEG:
            printf("NEG\n");
            ast_test(node->operand, depth + 1);
            break;
        case NODE_IDENTIFIER:
            printf("IDENTIFIER \"%s\"\n", node->name);
            break;
        case NODE_VAR_DECL:
            printf("VAR_DECLARE \"%s\"\n", node->name);
            ast_test(node->operand, depth + 1);
            break;
        case NODE_ASSIGN:
            printf("ASSIGN \"%s\"\n", node->name);
            ast_test(node->operand, depth + 1);
            break;
        case NODE_PRINT:
            printf("PRINT\n");
            ast_test(node->operand, depth + 1);
            break;
        case NODE_NOT:
            printf("NOT\n");
            ast_test(node->operand, depth + 1);
            break;
        case NODE_BLOCK:
            {
            printf("BLOCK\n");
            int n = node->stmt_count;
            for (int i = 0; i < n; i++) {
                ast_test(node->statements[i], depth + 1);
            }
            break;
            }
        case NODE_IF:
            printf("IF\n");
            for (int i = 0; i < depth; i++) printf("  ");
            printf("COND\n");
            ast_test(node->condition, depth + 2);
            for (int i = 0; i < depth; i++) printf("  ");
            printf("THEN\n");
            ast_test(node->then_branch, depth + 2);
            if (node->else_branch != NULL) {
                printf("ELSE\n");
                ast_test(node->else_branch, depth + 2);
            }
            break;
    }
}

void program_test(Program *prgm) {
    for (int i = 0; i < prgm->count; i++) {
        printf("[%d]\n", i);
        ast_test(prgm->statements[i], 1);
    }
}
