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
    TOKEN_PRINT, 
    TOKEN_IF, 
    TOKEN_ELSE, 
    TOKEN_EQUAL, 
    TOKEN_NOT_EQUAL, 
    TOKEN_LESS_THAN, 
    TOKEN_LESS_THAN_EQUAL, 
    TOKEN_GREATER_THAN, 
    TOKEN_GREATER_THAN_EQUAL, 
    TOKEN_LOGIC_NOT, 
    TOKEN_LOGIC_AND, 
    TOKEN_AND, 
    TOKEN_LOGIC_OR, 
    TOKEN_OR, 
    TOKEN_LBRACE, 
    TOKEN_RBRACE, 
    TOKEN_WHILE
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
