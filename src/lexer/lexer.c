#include <ctype.h>
#include <stddef.h>
#include <string.h>

#include "lexer.h"

const char *KEYWORDS[] = {
    "exit",
    "mov",
    "add",
    "sub",
    "jz",
    "jnz",
    "jmp",
    "write",
    "h",
    "he",
    "li",
    "be",
    "b",
    "c",
    "n",
    "o"
};
size_t KEYWORDSCOUNT = sizeof(KEYWORDS) / sizeof(KEYWORDS[0]);

void tokenize(const char *code, TokenArray *tokens) {
    int idx = 0;

    while (code[idx] != '\0') {

        if (isspace(code[idx])) {
            idx++;
            continue;
        }
        
        if (code[idx] == ';') {
            while (code[idx] != '\n' && code[idx] != '\0')
                idx++;
            continue;
        }

        if (isalpha(code[idx])) {
            char word[256];
            int wordIdx = 0;

            while (isalnum(code[idx])) {
                word[wordIdx++] = code[idx++];
            }

            word[wordIdx] = '\0';

            if (code[idx] == ':') {
                addTok(tokens, TOKEN_LABEL_DEF, word);
                idx++;
            }

            else if (contains(KEYWORDS, KEYWORDSCOUNT, word)) {
                addTok(tokens, TOKEN_KEYWORD, word);
            }
            else {
                addTok(tokens, TOKEN_LABEL_REF, word);
            }

            continue;
        }

        if (isdigit(code[idx])) {
            char number[256];
            int numIdx = 0;

            while (isdigit(code[idx])) {
                number[numIdx++] = code[idx++];
            }

            number[numIdx] = '\0';

            addTok(tokens, TOKEN_NUMBER, number);

            continue;
        }

        idx++;
    }
}