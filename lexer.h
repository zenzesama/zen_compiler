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
    TOKEN_EOF, 
    TOKEN_ASSIGN, 
    TOKEN_IDENTIFIER, 
    TOKEN_VAR, 
    TOKEN_INT, 
    TOKEN_PRINT
} TokenType;

typedef struct {
    TokenType type;
    int value;
    char text[64];
} Token;

typedef struct {
    const char *src;
    int pos;
} Lexer;

void lexer_init(Lexer *lex, const char *source);
Token lexer_next(Lexer *lex);

#endif
