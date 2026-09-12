#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

typedef struct {
    Lexer *lexer;
    Token current;
} Parser;

void parser_init(Parser *parser, Lexer *lex);
void parser_start(Parser *parser, Program *program);

#endif
