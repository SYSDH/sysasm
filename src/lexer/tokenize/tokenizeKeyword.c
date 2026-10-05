#include <ctype.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#include "lexer/lexer.h"

static inline int parseInt(const char *str) {
    char *end;
    errno = 0;

    long value = strtol(str, &end, 10);

    if (end == str || *end != '\0') return -1;

    if (errno == ERANGE || value > INT_MAX || value < INT_MIN) return -1;

    return (int)value;
}

int tokenizeKeyword(Lexer *lexer) {
    const char *code   = lexer->code;

    if (isalpha(code[lexer->idx]) || code[lexer->idx] == '_') {
        size_t cap     = 256;
        size_t wordIdx = 0;
        char *word     = malloc(cap);

        int startLine = lexer->line;
        int startCol  = lexer->col;

        while (isalnum(code[lexer->idx]) || code[lexer->idx] == '_') {
            if (wordIdx >= cap) {
                size_t newCap = cap * 2;

                char *temp = realloc(word, newCap);

                if (!temp) {
                    return 1;
                }

                word = temp;
                cap  = newCap;
            };

            word[wordIdx++] = advance_lexer(lexer);
        }

        word[wordIdx] = '\0';

        #define Y(kind_, str, search)                   \
            else if (                                   \
            !strcmp(search, word)) {                \
                addTok(lexer, kind_, word, wordIdx, startLine, startCol);    \
                \
            }

        #define X(...)
        #define Z(...)

        if (code[lexer->idx] == ':') {
            advance_lexer(lexer);
            addTok(lexer, TOKEN_LABEL_DEF, word, wordIdx, startLine, startCol);
        } TOKENS_TABLE else {
            addTok(lexer, TOKEN_LABEL_REF, word, wordIdx, startLine, startCol);
        }

        #undef X
        #undef Y
        #undef Z

        free(word);

        return 1;
    }

    return 0;
}