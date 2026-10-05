#include <string.h>
#include <ctype.h>

#include "lexer/lexer.h"

int tokenizeOperator(Lexer *lexer) {
    int startLine = lexer->line;
    int startCol  = lexer->col;

    const char *code   = lexer->code + lexer->idx;

    int bestLen = 0;
    TokenKind bestKind;

    #define Z(kind, str, op)                             \
        do {                                             \
            int len = sizeof(op) - 1;                    \
            \
            if (len > bestLen &&                         \
                strncmp(code, op, len) == 0) {           \
                bestLen = len;                           \
                bestKind = kind;                         \
            }                                            \
        } while (0);

    #define X(...)
    #define Y(...)

    TOKENS_TABLE

    #undef X
    #undef Y
    #undef Z

    if (bestLen == 0) return 0;

    addTok(lexer, bestKind, code, bestLen, startLine, startCol);

    for (int i = 0; i < bestLen; i++) advance_lexer(lexer);

    return 1;
}