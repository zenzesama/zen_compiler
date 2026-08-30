#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_NUMBER, 
    TOKEN_PLUS, 
    TOKEN_MINUS, 
    TOKEN_STAR, 
    TOKEN_SLASH, 
    TOKEN_PERCENT, 
    TOKEN_LPAREN, 
    TOKEN_RPAREN, 
    TOKEN_SEMICOLON, 
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    int value;
} Token;

typedef struct {
    const char *src;
    int pos;
} Lexer;

void lexer_init(Lexer *, const char *);
Token lexer_next(Lexer *);

#endif
