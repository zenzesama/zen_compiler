#ifndef DEBUG_H
#define DEBUG_H

#include "lexer.h"
#include "ast.h"

void lexer_test(Lexer *lex);
void ast_test(Node *node, int depth);

#endif
