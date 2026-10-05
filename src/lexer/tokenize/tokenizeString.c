#include <stddef.h>
#include <stdlib.h>

#include "lexer/lexer.h"

int tokenizeString(Lexer *lexer) {
    const char *code   = lexer->code;
    
    if (code[lexer->idx] == '"') {
        size_t cap = 256;

        char *str        = malloc(cap);
        size_t stringIdx = 0;

        int startLine = lexer->line;
        int startCol  = lexer->col;

        advance_lexer(lexer);

        while (code[lexer->idx] != '"' && code[lexer->idx] != '\0') {
            if (stringIdx >= cap) {
                size_t newCap = cap * 2;

                char *temp = realloc(str, newCap);

                if (!temp) {
                    free(str);
                    return 1;
                }

                cap = newCap;
                str = temp;
            }
            
            if (code[lexer->idx] == '\\') {
                advance_lexer(lexer);
                char esc = advance_lexer(lexer);

                switch (esc) {
                    case 'n':  str[stringIdx++] = '\n'; break;
                    case 't':  str[stringIdx++] = '\t'; break;
                    case 'r':  str[stringIdx++] = '\r'; break;
                    case '0':  str[stringIdx++] = '\0'; break;
                    case '\\': str[stringIdx++] = '\\'; break;
                    case '"':  str[stringIdx++] = '"';  break;
                    default:   str[stringIdx++] = esc;   break;
                }
            } else  {
                str[stringIdx++] = advance_lexer(lexer);
            }
        }

        if (code[lexer->idx] == '"') advance_lexer(lexer);

        addTok(lexer, TOKEN_STRING, str, stringIdx, startLine, startCol);
        free(str);
        
        return 1;
    }

    return 0;
}