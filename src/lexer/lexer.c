#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "lexer/lexer.h"
#include "args/args.h"
#include "utils.h"

void tokenize(const char *code, TokenArray *tokens, ArgCtx *ctx) {
    if (!code) return;

    Lexer lexer = {.code = code, .idx = 0, .tokens = tokens, .line = 1, .col = 1};

    while (code[lexer.idx] != '\0') {
        if (code[lexer.idx] == '.') {
            size_t cap     = 256;
            size_t dirIdx  = 0;
            char *dir      = malloc(cap);

            int startLine = lexer.line;
            int startCol  = lexer.col;

            while (code[lexer.idx] != '\0' && !isspace(code[lexer.idx])) {
                if (dirIdx >= cap) {
                    size_t newCap = cap * 2;

                    char *temp = realloc(dir, newCap);

                    if (!temp) {
                        return;
                    }

                    dir = temp;
                    cap  = newCap;
                };

                dir[dirIdx++] = advance_lexer(&lexer);
            }

            dir[dirIdx] = '\0';

            addTok(&lexer, TOKEN_DIRECTIVE, dir, dirIdx, startLine, startCol);
            logVerbose(ctx, "green", "LEXER", "Reading directive: '%s'", dir);

            continue;
        }

        if (skipTrash(&lexer, ";")) continue;

        #define X(handler, ...) \
            if (handler(&lexer)) continue;

        TOKENIZE_HELP_TABLE
        
        #undef X

  
        if (code[lexer.idx] != '\0') {
            int l = lexer.line;
            int c = lexer.col;

            char current = advance_lexer(&lexer);
            
            // addTok uses memcpy, so it is safe to use tostring here.
            addTok(&lexer, TOKEN_UNDEFINED, tostring(current), 1, l, c); 
        }
    }
}