#pragma once

#include "structs.h"

#define TOKENIZE_HELP_TABLE \
    X(tokenizeDigit     )   \
    X(tokenizeOperator  )   \
    X(tokenizeString    )   \
    X(tokenizeKeyword   )   \

#define X(handler) int handler(Lexer *lexer);
TOKENIZE_HELP_TABLE
#undef X

int skipTrash(Lexer *lexer, const char *commentStr);

static inline char advance_lexer(Lexer *lexer) {
    char current = lexer->code[lexer->idx++];

    if (current == '\n') {
        lexer->line++;
        lexer->col = 1;
    } else {
        lexer->col++;
    }

    return current;
}

static inline char *tostring(const char c) {
// WARNING: Misusing this function can result in an invalid or changing pointer.
// Use only as intended.

    static char str[2] = {0};
    str[0] = c;

    return str;
}

