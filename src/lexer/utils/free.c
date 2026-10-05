#include <stdlib.h>

#include "lexer/lexer.h"

void freeTokens(TokenArray *tokens) {
    for (size_t idx = 0; idx < tokens->size; idx++) {
        Token *tok = &tokens->data[idx];
        tok->kind  = TOKEN_UNDEFINED;
        
        free(tok->value);
    }

    free(tokens->data);

    tokens->capacity = 2;
    tokens->size     = 0;
}