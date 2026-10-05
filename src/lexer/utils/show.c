#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "lexer/lexer.h"

static inline int appendf(char **buffer, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    size_t currentLen = *buffer ? strlen(*buffer) : 0;

    va_list copy;
    va_copy(copy, args);

    int needed = vsnprintf(NULL, 0, fmt, copy);

    va_end(copy);

    if (needed < 0) {
        va_end(args);
        return -1;
    }

    char *tmp = realloc(*buffer, currentLen + (size_t)needed + 1);

    if (!tmp) {
        va_end(args);
        return -1;
    }

    *buffer = tmp;

    vsnprintf(
        *buffer + currentLen,
        (size_t)needed + 1,
        fmt,
        args
    );

    va_end(args);

    return needed;
}

void showTokens(const TokenArray *tokens) {
    #define Y(...) 
    #define Z X

    char *buff = NULL;

    #define X(type, str, ...) case type: { if (appendf(&buff, "TOKEN_" str "\n", t.value) == -1) { free(buff); return; } break; }

    for (size_t i = 0; i < tokens->size; i++) {
        Token t = tokens->data[i];

        switch (t.kind) {
            TOKENS_TABLE
            case TOKEN_KEYWORD: {
                if (appendf(&buff, "TOKEN_KEYWORD(%s)" "\n", t.value) == -1) { free(buff); return; }
                break;
            }
        }
    }

    printf("%s\n", buff);

    free(buff);
    
    #undef X
    #undef Y
    #undef Z
}