#include <stdio.h>
#include <ctype.h>

void preprocess(char code[]) {
    
    int ptr = 0;

    while (code[ptr] != '\0') {
        code[ptr] = tolower(code[ptr]);

        ptr++;
    }
}