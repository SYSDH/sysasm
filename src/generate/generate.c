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
    {"push",  PUSH,  PUSH_REG},
    {"load",  LOAD,  LOAD_REG},
    {"store", STORE, STORE_REG},
    {"jmp",   JMP,   JMP_REG},
    {"jz",    JZ,    0xFF},
    {"in",    IN,    0xFF},
    {"jnz",   JNZ,   0xFF},
    {"jg",    JG,   0xFF},
    {"jl",    JL,   0xFF},
    {"jle",   JLE,   0xFF},
    {"jge",   JGE,   0xFF},
    {"call",  CALL,   0xFF},
    {"ret",   RET,  0xFF},
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
    int currentAddress = 5;
    char entryPointLabel[256] = "_main";

    for (int i = 0; i < tokens.size; i++) {
        if (tokens.data[i].type == TOKEN_DIRECTIVE) {
            if (strcmp(tokens.data[i].value, ".entry") == 0) {
                if (i + 1 < tokens.size) {
                    strncpy(entryPointLabel, tokens.data[i+1].value, 255);
                    i++; 
                }
            }
            continue;
        }

        if (tokens.data[i].type == TOKEN_LABEL_DEF) {
            if (labelCount >= MAX_LABEL) {
                showError(FATAL_ERROR, "%d.%d: max label limit reached (%d)",
                    tokens.data[i].ln, tokens.data[i].col,
                    MAX_LABEL
                );

                return 1;
            }

            strncpy(labelTable[labelCount].name, tokens.data[i].value, sizeof(labelTable[labelCount].name) - 1);
            labelTable[labelCount].name[sizeof(labelTable[labelCount].name) - 1] = '\0';
            labelTable[labelCount].address = currentAddress;

             logVerbose(cfg, "magenta", "GENERATE", "Mapping label '%s' to address 0x%04X", 
               labelTable[labelCount].name, currentAddress);
            labelCount++;

            continue;
        } 
        else if (tokens.data[i].type == TOKEN_POINTER) continue;
        else if (tokens.data[i].type == TOKEN_NUMBER || tokens.data[i].type == TOKEN_LABEL_REF) {
            currentAddress += 4;
        }
        else if (tokens.data[i].type == TOKEN_STRING) { 
            int len = strlen(tokens.data[i].value) + 1;
            currentAddress += (len * 4); 
        }
        else if (tokens.data[i].type == TOKEN_KEYWORD && !strcmp(tokens.data[i].value, "db")) {
            continue;
        }
        else {
            currentAddress += 1;
        }
    }

    int totalBinarySize = currentAddress;
    int mainLabel = -1;
    
    for (int j = 0; j < labelCount; j++) {
        if (strcmp(entryPointLabel, labelTable[j].name) == 0) {
            mainLabel = labelTable[j].address;
            break;
        }
    }

    if (mainLabel == -1 && cfg.searchEntryPoint) {showError(FATAL_ERROR, "undefined reference to '_main' ou cannot find entry symbol _main"); return 1;};

    logVerbose(cfg, "magenta", "GENERATE", "Entry point set to _main at 0x%04X", mainLabel);

    FILE *file = fopen(cfg.outputName, "wb"); 

    if (!file) {
        showError(FATAL_ERROR, "error to generate binary file");
        return 1;
    }

    if (cfg.searchEntryPoint) {
        int jmp = JMP;
        fwrite(&jmp, 1, 1, file);
        fwrite(&mainLabel, 4, 1, file);
    }

    for (int i = 0; i < tokens.size; i++) {
        if (tokens.data[i].type == TOKEN_DIRECTIVE) {
            if (strcmp(tokens.data[i].value, ".entry") == 0) {
                i++;
            }
            continue;
        }

        switch (tokens.data[i].type) {
            case TOKEN_POINTER: case TOKEN_LABEL_DEF: case TOKEN_DIRECTIVE:
                continue;

            case TOKEN_KEYWORD: {
                int regidx = getRegIdx(tokens.data[i].value);

                if (regidx != -1) {
                    unsigned char byte = (unsigned char)regidx;

                    logVerbose(cfg, "magenta", "GENERATE", "0x%04lX: Register %s (0x%02X)", ftell(file), tokens.data[i].value, byte);

                    fwrite(&byte, 1, 1, file);
                    break;
                }

                if (strcmp(tokens.data[i].value, "db") == 0) continue; 

                int instIdx = -1;

                for (size_t j = 0; j < sizeof(instructionTable)/sizeof(instructionTable[0]); j++) {
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
                logVerbose(cfg, "magenta", "GENERATE", "0x%04lX: Opcode %s (0x%02X)", ftell(file), inst.name, opcode);

                fwrite(&opcode, 1, 1, file);
                break;
            }

            case TOKEN_NUMBER: {
                int val = atoi(tokens.data[i].value);
                
                if (i > 0 && tokens.data[i-1].type == TOKEN_KEYWORD) {
                    if (getOpNormalByName(instructionTable, sizeof(instructionTable)/sizeof(instructionTable[0]), tokens.data[i-1].value) == STORE && val < totalBinarySize) {
                        showError(WARNING_ERROR, "address %d is reserved (program code area)", val);
                    }
                }

                logVerbose(cfg, "magenta", "GENERATE", "0x%04lX: Immediate Value %d (Hex: 0x%08X)", ftell(file), val, val);
                fwrite(&val, 4, 1, file);
                break;
            }

            case TOKEN_STRING: {
                char *str = tokens.data[i].value;
                logVerbose(cfg, "magenta", "GENERATE", "Writing string '%s' at 0x%04lX", str, ftell(file));
                
                int len = strlen(str);

                for (int j = 0; j < len; j++) {
                    unsigned int byte = (unsigned char)str[j];
                    fwrite(&byte, 4, 1, file);
                }
                
                unsigned int nullTerm = 0;
                fwrite(&nullTerm, 4, 1, file);

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
                    logVerbose(cfg, "magenta", "GENERATE", "0x%04lX: Label Reference '%s' -> 0x%08X", ftell(file), tokens.data[i].value, found);
                    fwrite(&addr, 4, 1, file);
                } else {

                    showError(FATAL_ERROR, "%d.%d: undefined label: '%s'",
                        tokens.data[i].ln, tokens.data[i].col,
                        tokens.data[i].value);
                        
                    fclose(file);
                    return 1;
                }
            }
        }
    }

    fclose(file);
    return 0;
}
