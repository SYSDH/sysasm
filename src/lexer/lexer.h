#ifndef LEXER_H
#define LEXER_H

#include "../helpers/helpers.h"

extern const char *KEYWORDS[];
extern size_t KEYWORDSCOUNT;

typedef enum {
    TOKEN_KEYWORD,
    TOKEN_NUMBER,

    TOKEN_LABEL_REF,
    TOKEN_LABEL_DEF,

    TOKEN_POINTER,
} TokenType;

typedef struct {
    TokenType type;
    char value[256];

    int ln;
    int col;
} Token;

typedef struct {
    Token *data;
    int size;
    int capacity;
} TokenArray;

void tokenize(const char *code, TokenArray *tokens);
void addTok(TokenArray *tokens, TokenType type, const char *value);

#endif
