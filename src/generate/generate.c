#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "generate.h"

#include "../helpers/helpers.h"

const keywordsStruct keywordTable[] = {
    // Commands
    {"mov",   MOV},
    {"add",   ADD},
    {"jz",    JZ},
    {"jmp",   JMP},
    {"write", WRITE},
    {"exit",  EXIT},

    // Registers
    {"h",  H},
    {"he", He},
    {"li", Li},
    {"be", Be},
    {"b",  B},
    {"c",  C},
    {"n",  N},
    {"o",  O}
}; 
const int tableSize = sizeof(keywordTable) / sizeof(keywordTable[0]);

int generate(TokenArray tokens) {
    FILE *file = fopen("output.hex", "w");
    if (!file) {
        printf("Error to create file\n");
        return 1;
    }

    for (int i = 0; i < tokens.size; i++) {
        if (tokens.data[i].type == TOKEN_KEYWORD) {
            char *val = findValue(tokens.data[i].value);
            if (val) fprintf(file, "%s ", val);
        } 
        else if (tokens.data[i].type == TOKEN_NUMBER) {
            fprintf(file, "0x%X ", atoi(tokens.data[i].value));
        }
    }

    fclose(file);
    free(tokens.data);
    return 0;
}