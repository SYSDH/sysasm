#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "generate.h"

#include "../helpers/helpers.h"

const InstructionMap instructionTable[] = {
    {"mov",   MOV,   MOV_REG},
    {"add",   ADD,   ADD_REG},
    {"sub",   SUB,   SUB_REG},
    {"out",   OUT,   OUT_REG},
    {"in",    IN,    0xFF},
    {"push",  PUSH,  PUSH_REG},
    {"load",  LOAD,  LOAD_REG},
    {"store", STORE, STORE_REG},
    {"jmp",   JMP,   JMP_REG},
    {"jz",    JZ,    0xFF},
    {"jnz",   JNZ,   0xFF},
    {"pop",   POP,   0xFF},
    {"exit",  EXIT,  0xFF}
};


const RegisterMap registerTable[] = {
    {"h", 0}, {"he", 1}, {"li", 2}, {"be", 3}, 
    {"b", 4}, {"c", 5}, {"n", 6}, {"o", 7}
};

LabelSymbol labelTable[MAX_LABEL];

int labelCount = 0;

int getRegIdx(const char* name) {
    for (int i = 0; i < 8; i++) {
        if (strcmp(name, registerTable[i].name) == 0) return i;
    }

    return -1;
}

int getOpNormalByName(const InstructionMap *array, int size, const char *searchName) {
    for (int i = 0; i < size; i++) {
        if (strcmp(array[i].name, searchName) == 0) {
            return array[i].opNormal;
        }
    }
    return -1;
}

int generate(TokenArray tokens, Config cfg) {
    labelCount = 0;
    int currentAddress = 0;

    for (int i = 0; i < tokens.size; i++) {
        if (tokens.data[i].type == TOKEN_LABEL_DEF) {
            if (labelCount >= MAX_LABEL) {
                char buff[512];

                snprintf(buff, sizeof(buff), 
                    "%d.%d: max label limit reached (%d)",
                    tokens.data[i].ln, tokens.data[i].col,
                    MAX_LABEL
                );

                showError(FATAL_ERROR, buff);

                return 1;
            }

            strncpy(labelTable[labelCount].name, tokens.data[i].value, sizeof(labelTable[labelCount].name) - 1);
            labelTable[labelCount].name[sizeof(labelTable[labelCount].name) - 1] = '\0';
            labelTable[labelCount].address = currentAddress;
            labelCount++;
        } 
        else if (tokens.data[i].type == TOKEN_POINTER) continue;
        else if (tokens.data[i].type == TOKEN_NUMBER || tokens.data[i].type == TOKEN_LABEL_REF) {
            currentAddress += 4;
        } 
        else {
            currentAddress += 1;
        }
    }

    int totalBinarySize = currentAddress;

    FILE *file = fopen(cfg.outputName, "wb"); 

    if (!file) {
        showError(FATAL_ERROR, "error to generate binary file");
        return 1;
    }

    for (int i = 0; i < tokens.size; i++) {

        switch (tokens.data[i].type) {
            case TOKEN_POINTER:
                continue;

            case TOKEN_KEYWORD: {
                int regidx = getRegIdx(tokens.data[i].value);

                if (regidx != -1) {
                    unsigned char byte = (unsigned char)regidx;

                    fwrite(&byte, 1, 1, file);
                    break;
                }

                int instIdx = -1;

                for (int j = 0; j < sizeof(instructionTable)/sizeof(instructionTable[0]); j++) {
                    if (strcmp(tokens.data[i].value, instructionTable[j].name) == 0) {
                        instIdx = j; break;
                    }
                }

                if (instIdx == -1) continue;

                InstructionMap inst = instructionTable[instIdx];
                int useRegVersion = 0;
                
                if (i + 1 < tokens.size && tokens.data[i+1].type == TOKEN_POINTER) {
                    useRegVersion = 1;
                } 

                else if (strcmp(inst.name, "mov") == 0 || strcmp(inst.name, "add") == 0 || strcmp(inst.name, "sub") == 0) {
                    if (i + 2 < tokens.size && getRegIdx(tokens.data[i+2].value) != -1) {
                        useRegVersion = 1;
                    }
                }

                unsigned char opcode = useRegVersion ? inst.opReg : inst.opNormal;
                fwrite(&opcode, 1, 1, file);
                break;
            }

            case TOKEN_NUMBER: {
                int val = atoi(tokens.data[i].value);
                
                if (i > 0 && tokens.data[i-1].type == TOKEN_KEYWORD) {
                    if (getOpNormalByName(instructionTable, sizeof(instructionTable)/sizeof(instructionTable[0]), tokens.data[i-1].value) == STORE && val < totalBinarySize) {
                        char buff[256];
                        snprintf(buff, sizeof(buff), "address %d is reserved (program code area)", val);
                        showError(WARNING_ERROR, buff);
                    }
                }

                fwrite(&val, 4, 1, file);
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
                    int addr = found;
                    fwrite(&addr, 4, 1, file);
                } else {
                    char buff[512];

                    snprintf(buff, sizeof(buff), 
                        "%d.%d: undefined label: '%s'",
                        tokens.data[i].ln, tokens.data[i].col,
                        tokens.data[i].value
                    );

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