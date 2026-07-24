#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "lexer.h"

void lexer_init(Lexer *lex, const char *source) {
    lex->src = source;
    lex->pos = 0;
}

static char peek(Lexer *lex) {
    return lex->src[lex->pos];
}

static char advance(Lexer *lex) {
    return lex->src[lex->pos++];
}

Token lexer_next(Lexer *lex) {
    while (isspace((unsigned char)peek(lex))) {
        advance(lex);
    }

    char c = peek(lex);
    Token t;

    if (c == '\0') {
        t.type = TOKEN_EOF;
        t.value = 0;
        return t;
    }

    if (isdigit((unsigned char)c)) {
        int start = lex->pos;

        while (isdigit((unsigned char)peek(lex))) {
            advance(lex);
        }

        int len = lex->pos - start;
        char buff[64];

        if (len >= (int)sizeof(buff)) 
            len = sizeof(buff) - 1;

        for (int i = 0; i < len; i++) 
            buff[i] = lex->src[start + i];
        buff[len] = '\0';

        t.type = TOKEN_NUMBER;
        t.value = atoi(buff);

        return t;
    }

    advance(lex);

    switch (c) {
        case '+':
            t.type = TOKEN_PLUS;
            break;
        case '-':
            t.type = TOKEN_MINUS;
            break;
        case '*':
            t.type = TOKEN_STAR;
            break;
        case '/':
            t.type = TOKEN_SLASH;
            break;
        case '%':
            t.type = TOKEN_PERCENT;
            break;
        case '(':
            t.type = TOKEN_LPAREN;
            break;
        case ')':
            t.type = TOKEN_RPAREN;
            break;
        case ';':
            t.type = TOKEN_SEMICOLON;
            break;
        default:
            fprintf(stderr, "Lexer error: Unexpected token %c.\n", c);
            exit(1);
    }

    return t;
}
