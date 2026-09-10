#include <stdio.h>
#include "debug.h"

static const char *token_type_name(TokenType type) {
    switch (type) {
        case TOKEN_NUMBER:     return "NUMBER";
        case TOKEN_PLUS:       return "PLUS";
        case TOKEN_MINUS:      return "MINUS";
        case TOKEN_STAR:       return "STAR";
        case TOKEN_SLASH:      return "SLASH";
        case TOKEN_PERCENT:    return "PERCENT";
        case TOKEN_LPAREN:     return "LPAREN";
        case TOKEN_RPAREN:     return "RPAREN";
        case TOKEN_SEMICOLON:  return "SEMICOLON";
        case TOKEN_EOF:        return "EOF";
        case TOKEN_ASSIGN:     return "ASSIGN";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_VAR:        return "VAR";
        case TOKEN_INT:        return "INT";
        default:               return "UNKNOWN";
    }
}

static void print_token(Token t) {
    printf("%-12s", token_type_name(t.type));
    if (t.type == TOKEN_NUMBER) printf(" value=%d", t.value);
    if (t.type == TOKEN_IDENTIFIER) printf(" text=\"%s\"", t.text);
    printf("\n");
}

void lexer_test(Lexer *lex) {
    while (1) {
        Token t = lexer_next(lex);
        print_token(t);
        if (t.type == TOKEN_EOF) break;
    }
}

void ast_test(Node *node, int depth) {
    if (!node) return;

    for (int i = 0; i < depth; i++) printf("  ");

    switch (node->type) {
        case NODE_NUMBER:
            printf("NUMBER %d\n", node->number);
            break;
        case NODE_BINARYOP:
            printf("BINARY OP '%c'\n", node->op);
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
    }
}
