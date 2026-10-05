#include <ctype.h>
#include <stdlib.h>
#include <stddef.h>

#include "lexer/lexer.h"

int tokenizeDigit(Lexer *lexer) {
    const char *code   = lexer->code;
    const char current = lexer->code[lexer->idx];

    if (isdigit(current)) {
        size_t cap     = 256;
        size_t numIdx  = 0;
        char *number   = malloc(cap);

        int startLine = lexer->line;
        int startCol  = lexer->col;
        
        while (isdigit(code[lexer->idx])) {
            if (numIdx >= cap) {
                size_t newCap = cap * 2;

                char *temp = realloc(number, newCap);

                if (!temp) {
                    free(number);
                    return 1;
                }

                number = temp;
                cap  = newCap;
            };

            number[numIdx++] = advance_lexer(lexer);
        }

        number[numIdx] = '\0';
        
        addTok(lexer, TOKEN_NUMBER, number, numIdx, startLine, startCol);

        free(number);
        
        return 1;
    }

    return 0;
}