#include <stdio.h>
#include <stdlib.h>
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

static Node *parse_expression(Parser *p);
static Node *parse_term(Parser *p);
static Node *parse_factor(Parser *p);
static Node *parse_primary(Parser *p);

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
        Node *right = parse_term(p);
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
    fprintf(stderr, "Parser error: Expected a number or (.\n");
    exit(1);
}

Node *parser_start(Parser *p) {
    Node *expr = parse_expression(p);
    expect(p, TOKEN_SEMICOLON, "';'");
    return expr;
}
