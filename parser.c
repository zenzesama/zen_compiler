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

static Node *parse_expression(Parser *p);
static Node *parse_statement(Parser *p);
static Node *parse_var_decl(Parser *p);
static Node *parse_expr_stmt(Parser *p);
static Node *parse_print_stmt(Parser *p);
static Node *parse_block(Parser *p);
static Node *parse_while_stmt(Parser *p);
static Node *parse_if_stmt(Parser *p);
static Node *parse_logic_or(Parser *p);
static Node *parse_logic_and(Parser *p);
static Node *parse_logic_equality(Parser *p);
static Node *parse_logic_comparison(Parser *p);
static Node *parse_additive(Parser *p);
static Node *parse_term(Parser *p);
static Node *parse_factor(Parser *p);
static Node *parse_primary(Parser *p);

static Node *parse_expression(Parser *p) {
    return parse_logic_or(p);
}

static Node *parse_statement(Parser *p) {
    switch(p->current.type) {
        case TOKEN_PRINT:
            return parse_print_stmt(p);
        case TOKEN_VAR:
            return parse_var_decl(p);
        case TOKEN_IF:
            return parse_if_stmt(p);
        case TOKEN_LBRACE:
            return parse_block(p);
        case TOKEN_WHILE:
            return parse_while_stmt(p);
        default:
            return parse_expr_stmt(p);
    }
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

static Node *parse_print_stmt(Parser *p) {
    expect(p, TOKEN_PRINT, "'print'");
    expect(p, TOKEN_LPAREN, "'('");
    Node *expr = parse_expression(p);
    expect(p, TOKEN_RPAREN, "')'");
    expect(p, TOKEN_SEMICOLON, "';'");
    return make_print(expr);
}

static Node *parse_block(Parser *p) {
    expect(p, TOKEN_LBRACE, "'{'");
    Node *b = make_block();
    while (p->current.type != TOKEN_RBRACE) {
        if (p->current.type == TOKEN_EOF) {
            fprintf(stderr, "Parser error: Expected '}'\n");
            exit(1);
        }
        block_add(b, parse_statement(p));
    }
    expect(p, TOKEN_RBRACE, "'}'");
    return b;
}

static Node *parse_while_stmt(Parser *p) {
    expect(p, TOKEN_WHILE, "\"while\"");
    expect(p, TOKEN_LPAREN, "'('");
    Node *cond = parse_expression(p);
    expect(p, TOKEN_RPAREN, "')'");
    Node *body = parse_block(p);
    return make_while(cond, body);
}

static Node *parse_if_stmt(Parser *p) {
    expect(p, TOKEN_IF, "'if'");
    expect(p, TOKEN_LPAREN, "'('");
    Node *cond = parse_expression(p);
    expect(p, TOKEN_RPAREN, "')'");
    Node *then = parse_block(p);
    Node *else_b = NULL;
    if (p->current.type == TOKEN_ELSE) {
        advance(p);
        if (p->current.type == TOKEN_IF) {
            else_b = parse_if_stmt(p);
        }
        else {
            else_b = parse_block(p);
        }
    }
    return make_if(cond, then, else_b);
}

static Node *parse_logic_or(Parser *p) {
    Node *left = parse_logic_and(p);
    while (p->current.type == TOKEN_LOGIC_OR) {
        OpKind op = OP_OR;
        advance(p);
        Node *right = parse_logic_and(p);
        left = make_binaryop(op, left, right);
    }
    return left;
}

static Node *parse_logic_and(Parser *p) {
    Node *left = parse_logic_equality(p);
    while (p->current.type == TOKEN_LOGIC_AND) {
        OpKind op = OP_AND;
        advance(p);
        Node *right = parse_logic_equality(p);
        left = make_binaryop(op, left, right);
    }
    return left;
}

static Node *parse_logic_equality(Parser *p) {
    Node *left = parse_logic_comparison(p);
    while (p->current.type == TOKEN_EQUAL ||
           p->current.type == TOKEN_NOT_EQUAL) {
        OpKind op = (p->current.type == TOKEN_EQUAL ? OP_EQ : OP_NEQ);
        advance(p);
        Node *right = parse_logic_comparison(p);
        left = make_binaryop(op, left, right);
    }
    return left;
}

static Node *parse_logic_comparison(Parser *p) {
    Node *left = parse_additive(p);
    while (p->current.type == TOKEN_LESS_THAN ||
           p->current.type == TOKEN_LESS_THAN_EQUAL ||
           p->current.type == TOKEN_GREATER_THAN ||
           p->current.type == TOKEN_GREATER_THAN_EQUAL) {
        OpKind op = (p->current.type == TOKEN_LESS_THAN ? OP_LT : 
                    (p->current.type == TOKEN_LESS_THAN_EQUAL ? OP_LTE :
                    (p->current.type == TOKEN_GREATER_THAN ? OP_GT : OP_GTE)));
        advance(p);
        Node *right = parse_additive(p);
        left = make_binaryop(op, left, right);
    }
    return left;
}

static Node *parse_additive(Parser *p) {
    Node *left = parse_term(p);
    while (p->current.type == TOKEN_PLUS || p->current.type == TOKEN_MINUS) {
        OpKind op = (p->current.type == TOKEN_PLUS) ? OP_ADD : OP_SUB;
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
        OpKind op = (p->current.type == TOKEN_STAR ? OP_MUL 
                : (p->current.type == TOKEN_SLASH ? OP_DIV : OP_MOD));
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
    if (p->current.type == TOKEN_LOGIC_NOT) {
        advance(p);
        return make_not(parse_factor(p));
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
