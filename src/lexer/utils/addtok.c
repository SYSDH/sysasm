#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer/lexer.h"
#include "utils.h"

void addTok(Lexer *lexer, TokenKind kind, const char *value, int len, int line, int col) {
    TokenArray *tokens = lexer->tokens;

    if (tokens->size >= tokens->capacity) {
        tokens->capacity *= 2;
        tokens->data = realloc(tokens->data, 
                               tokens->capacity * sizeof(Token));

        if (!tokens->data) {
            showError(FATAL_ERROR, "error to realloc memory");
            exit(1);
        }
    }

    Token *tok  = &tokens->data[tokens->size];

    tok->kind   = kind;
    tok->line   = line;
    tok->col    = col;
    
    tok->value  = malloc(len + 1);

    if (tok->value) {
        memcpy(tok->value, value, len);
        tok->value[len] = '\0';
    }
    
    tokens->size++;
}