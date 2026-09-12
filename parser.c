#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

static void advance(Parser *p) {
    p->current = lexer_next(p->lexer);
}

static void expect(Parser *p, TokenType type, char *desc) {
    if (p->current.type != type) {
        fprintf(stderr, "Parser error: expected %s\n", desc);
        exit(1);
    }
    advance(p);
}

void parser_init(Parser *p, Lexer *l) {
    p->lexer = l;
    advance(p);
}

static Node *parse_statement(Parser *p);
static Node *parse_var_decl(Parser *p);
static Node *parse_expr_stmt(Parser *p);
static Node *parse_expression(Parser *p);
static Node *parse_term(Parser *p);
static Node *parse_factor(Parser *p);
static Node *parse_primary(Parser *p);

static Node *parse_statement(Parser *p) {
    if (p->current.type == TOKEN_VAR) {
        return parse_var_decl(p);
    }
    return parse_expr_stmt(p);
}

static Node *parse_var_decl(Parser *p) {
    expect(p, TOKEN_VAR, "'var'");
    expect(p, TOKEN_INT, "'int'");
    char name[64];
    strcpy(name, p->current.text);
    expect(p, TOKEN_IDENTIFIER, "'identifier'");

    Node *init = NULL;
    if (p->current.type == TOKEN_ASSIGN) {
        advance(p);
        init = parse_expression(p);
    }
    expect(p, TOKEN_SEMICOLON, "';'");
    return make_var_decl(name, init);
}

static Node *parse_expr_stmt(Parser *p) {
    Node *expr = parse_expression(p);
    if (expr->type == NODE_IDENTIFIER && p->current.type == TOKEN_ASSIGN) {
        char name[64];
        strcpy(name, expr->name);
        advance(p);

        Node *rhs = parse_expression(p);
        free_node(expr);
        
        expr = make_assign(name, rhs);
    }
    expect(p, TOKEN_SEMICOLON, "';'");
    return expr;
}

static Node *parse_expression(Parser *p) {
    Node *left = parse_term(p);
    while (p->current.type == TOKEN_PLUS || p->current.type == TOKEN_MINUS) {
        char op = (p->current.type == TOKEN_PLUS) ? '+' : '-';
        advance(p);
        Node *right = parse_term(p);
        left = make_binaryop(op, left, right);
    }
    return left;
}

static Node *parse_term(Parser *p) {
    Node *left = parse_factor(p);
    while (p->current.type == TOKEN_STAR ||
           p->current.type == TOKEN_SLASH || 
           p->current.type == TOKEN_PERCENT) {
        char op = (p->current.type == TOKEN_STAR ? '*' 
                : (p->current.type == TOKEN_SLASH ? '/' : '%'));
        advance(p);
        Node *right = parse_factor(p);
        left = make_binaryop(op, left, right);
    }
    return left;
}

static Node *parse_factor(Parser *p) {
    if (p->current.type == TOKEN_MINUS) {
        advance(p);
        return make_neg(parse_factor(p));
    }
    return parse_primary(p);
}

static Node *parse_primary(Parser *p) {
    if (p->current.type == TOKEN_NUMBER) {
        Node *n = make_number(p->current.value);
        advance(p);
        return n;
    }
    if (p->current.type == TOKEN_LPAREN) {
        advance(p);
        Node *n = parse_expression(p);
        expect(p, TOKEN_RPAREN, "')'");
        return n;
    }
    if (p->current.type == TOKEN_IDENTIFIER) {
        Node *n = make_identifier(p->current.text);
        advance(p);
        return n;
    }
    fprintf(stderr, "Parser error: Expected a number, '(' or Identifier.\n");
    exit(1);
}

void parser_start(Parser *p, Program *prgm) {
    prgm->count = 0;
    while (p->current.type != TOKEN_EOF) {
        if (prgm->count >= 256) {
            fprintf(stderr, "Parser error: Program can't be more than 256 statements.\n");
            exit(1);
        }
        prgm->statements[prgm->count++] = parse_statement(p);
    }
}
