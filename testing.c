#include <stdio.h>
#include "lexer.h"

void print_lex(Lexer *lex) {
    Token end;
    end.type = TOKEN_EOF;
    end.value = 0;
    
    Token t;
    while (1) {
        t = lexer_next(lex);
        if (t.type == TOKEN_EOF) break;

        fprintf(stdout, "TokenType = %d", t.type);
        if (t.type == 0)
            fprintf(stdout, " TokenValue = %d", t.value);
        fprintf(stdout, "\n");
    }
}
