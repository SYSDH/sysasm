#pragma once

#include "kind.h"

typedef struct Token {
    TokenKind kind;
    char*     value;

    int col;
    int line;
} Token;

typedef struct TokenArray {
    Token *data;
    size_t size;
    size_t capacity;
} TokenArray;

typedef struct Lexer {
    TokenArray *tokens;
    const char *code;
    size_t     idx;

    int col;
    int line;
} Lexer;


