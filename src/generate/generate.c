#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "generate.h"

#include "../helpers/helpers.h"

const keywordsStruct keywordTable[] = {
    // Commands
    {"mov",   MOV},
    {"add",   ADD},
    {"sub",   SUB},
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

LabelSymbol labelTable[100];

const int tableSize = sizeof(keywordTable) / sizeof(keywordTable[0]);
int labelCount = 0;

int generate(TokenArray tokens, Config cfg) {
    labelCount = 0;
    int currentAddress = 0;

    for (int i = 0; i < tokens.size; i++) {
        if (tokens.data[i].type == TOKEN_LABEL_DEF) {
            strncpy(labelTable[labelCount].name, tokens.data[i].value, sizeof(labelTable[labelCount].name));
            labelTable[labelCount].address = currentAddress;
            labelCount++;
        } 
        else if (tokens.data[i].type == TOKEN_KEYWORD || 
                 tokens.data[i].type == TOKEN_NUMBER ||
                 tokens.data[i].type == TOKEN_LABEL_REF) {

            currentAddress++;
        }
    }

    FILE *file = fopen(cfg.outputName, "wb"); 
    if (!file) {
        showError(FATAL_ERROR, "error to generate binary file");
        return 1;
    }

    for (int i = 0; i < tokens.size; i++) {
        switch (tokens.data[i].type) {
            case TOKEN_KEYWORD: {
                char *valStr = findValue(tokens.data[i].value);
                
                if (valStr) {
                    unsigned char byte = (unsigned char)strtol(valStr, NULL, 16);
                    fwrite(&byte, 1, 1, file); 
                }
                break;
            } 
            case TOKEN_NUMBER: {

                unsigned char byte = (unsigned char)atoi(tokens.data[i].value);
                fwrite(&byte, 1, 1, file);
                break;
            }

            case TOKEN_LABEL_REF: {
                int found = -1;

                for (int j = 0; j < labelCount; j++) {
                    if (strcmp(tokens.data[i].value, labelTable[j].name) == 0) {
                        found = labelTable[j].address;
                        break;
                    }
                }

                if (found != -1) {
                    unsigned char byte = (unsigned char)found;
                    fwrite(&byte, 1, 1, file);
                } else {
                    char buff[512];

                    snprintf(buff, sizeof(buff), "undefined label: '%s'", tokens.data[i].value);
                    showError(FATAL_ERROR, buff);
                    fclose(file);
                    return 1;
                }
            }
        }
    }

    fclose(file);
    return 0;
}