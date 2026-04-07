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
    "jg",
    "jl",
    "jle",
    "jge",
    "jmp",
    "call",
    "ret",
    "out",
    "in",
    "load",
    "store",
    "push",
    "pop",
    "db",

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

void tokenize(const char *code, TokenArray *tokens, Config cfg) {
    int idx = 0;
    int ln = 1;
    int col = 1;

    while (code[idx] != '\0') {
        if (code[idx] == '\n') {
            ln++;
            col = 1;
            idx++;
            continue;
        }

        if (isspace(code[idx])) {
            idx++;
            col++;
            continue;
        }
        
        if (code[idx] == ';') {
            logVerbose(cfg, "green", "LEXER", "Skipping comment line");

            while (code[idx] != '\n' && code[idx] != '\0')
                idx++;
            continue;
        }

        if (code[idx] == '$') {
            addTok(tokens, TOKEN_POINTER, "$");
            idx++;
            col++;
            continue;
        }

        if (code[idx] == '.') {
            char dir[256];
            int dIdx = 0;
            int startCol = col;
            
            while (code[idx] != '\0' && !isspace(code[idx])) {
                dir[dIdx++] = code[idx++];
                col++;
            }
            dir[dIdx] = '\0';
            
            addTok(tokens, TOKEN_DIRECTIVE, dir);
            tokens->data[tokens->size-1].ln = ln;
            tokens->data[tokens->size-1].col = startCol;
            
            logVerbose(cfg, "green", "LEXER", "Reading directive: '%s'", dir);
            continue;
        }

        if (isalpha(code[idx]) || code[idx] == '_') {
            char word[256];
            int wordIdx = 0;
            int startCol = col;

            while (isalnum(code[idx]) || code[idx] == '_') {
                word[wordIdx++] = code[idx++];
                col++;
            }

            word[wordIdx] = '\0';

            if (code[idx] == ':') {
                addTok(tokens, TOKEN_LABEL_DEF, word);
                idx++;
                col++;
            }
            else if (contains(KEYWORDS, KEYWORDSCOUNT, word)) {
                addTok(tokens, TOKEN_KEYWORD, word);
            }
            else {
                addTok(tokens, TOKEN_LABEL_REF, word);
                
            }
            
            tokens->data[tokens->size-1].ln = ln;
            tokens->data[tokens->size-1].col = startCol;

            logVerbose(cfg, "green", "LEXER", "Reading word: '%s'", word);

            continue;
        }

        if (isdigit(code[idx])) {
            char number[256];
            int numIdx = 0;
            int startCol = col;

            while (isdigit(code[idx])) {
                number[numIdx++] = code[idx++];
            }

            number[numIdx] = '\0';

            addTok(tokens, TOKEN_NUMBER, number);

            tokens->data[tokens->size-1].ln = ln;
            tokens->data[tokens->size-1].col = startCol;

            logVerbose(cfg, "green", "LEXER", "Reading number: '%s'", number);
            
            continue;
        }


        if (code[idx] == '"') {
            char str[256] = {0};
            int stringIdx = 0;
            int startCol = col;

            idx++;
            col++;

            while (code[idx] != '"' && code[idx] != '\0') {
                if (code[idx] == '\\') {
                    idx++;
                    col++;

                    if (code[idx] == '\0') break;

                    switch (code[idx]) {
                        case 'n': str[stringIdx++] = '\n'; break;
                        case 't': str[stringIdx++] = '\t'; break;
                        case 'r': str[stringIdx++] = '\r'; break;
                        case '0': str[stringIdx++] = '\0'; break;
                        case '\\': str[stringIdx++] = '\\'; break;
                        case '"': str[stringIdx++] = '"'; break;
                        default:   
                            str[stringIdx++] = code[idx]; 
                            break;
                    }

                    idx++;
                    col++;
                }
                else {
                    str[stringIdx++] = code[idx++];
                    col++;
                }
            }

            if (code[idx] == '"') {idx++; col++;}

            addTok(tokens, TOKEN_STRING, str);

            tokens->data[tokens->size-1].ln = ln;
            tokens->data[tokens->size-1].col = startCol;

            logVerbose(cfg, "green", "LEXER", "Reading string: '%s'", str);
            
            continue;
        }

        idx++;
        col++;
    }
}