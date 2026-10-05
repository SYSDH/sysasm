#include <ctype.h>
#include <string.h>

#include "lexer/lexer.h"

int skipComment(Lexer *lexer, const char *commentStr) {
    size_t commentStrLen = strlen(commentStr);

    int equal = 1;

    for (size_t i = 0; i < commentStrLen; i++) {
        if (lexer->code[lexer->idx+i] != commentStr[i]) {equal = 0; break;}
    }

    if (equal) {
        while (lexer->code[lexer->idx] != '\n' && lexer->code[lexer->idx] != '\0') {
            advance_lexer(lexer);
        }

        return 1;
    }

    return 0;
}

int skipTrash(Lexer *lexer, const char *commentStr) {
    if (lexer->code[lexer->idx] == '\n') {
        advance_lexer(lexer);
        return 1;
    }

    if (isspace(lexer->code[lexer->idx])) {
        advance_lexer(lexer);
        return 1;
    }

    return skipComment(lexer, commentStr);
}