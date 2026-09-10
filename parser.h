#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

typedef struct {
    Lexer *lexer;
    Token current;
} Parser;

void parser_init(Parser *parser, Lexer *lex);
Node *parser_start(Parser *parser);

#endif
