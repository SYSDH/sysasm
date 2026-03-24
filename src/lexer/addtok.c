#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer.h"

void addTok(TokenArray *tokens, TokenType type, const char *value) {

    if (tokens->size >= tokens->capacity) {
        tokens->capacity *= 2;
        tokens->data = realloc(tokens->data, 
                               tokens->capacity * sizeof(Token));

        if (!tokens->data) {
            printf("Error to realloc\n");
            exit(1);
        }
    }

    tokens->data[tokens->size].type = type;
    snprintf(tokens->data[tokens->size].value,
             sizeof(tokens->data[tokens->size].value),
             "%s", value);

    tokens->size++;
}