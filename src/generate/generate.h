#ifndef GENERATE_H
#define GENERATE_H

#include <stddef.h>

#include "../lexer/lexer.h"
#include "../args/args.h"

#define MAX_LABEL 100

typedef enum {
    // System
    EXIT        = 0xFF,

    // Data
    MOV         = 0x10, MOV_REG     = 0x11,
    // Math
    ADD         = 0x20, ADD_REG     = 0x21,
    SUB         = 0x22, SUB_REG     = 0x23,
    // Flow
    JZ          = 0x30,
    JNZ         = 0x31,
    JMP         = 0x32, JMP_REG     = 0x33,
    CALL        = 0x34,
    RET         = 0x35,
    JG          = 0x36, JGE         = 0x37,
    JL          = 0x38, JLE         = 0x39,
    
    // I/O
    OUT         = 0x40, OUT_REG     = 0x41,
    IN          = 0x42,
    LOADF       = 0x43, LOADF_REG   = 0x44,
    // Stack
    PUSH        = 0x50, PUSH_REG    = 0x51,
    POP         = 0x52,
    // Ram
    LOAD        = 0x60, LOAD_REG    = 0x61,
    STORE       = 0x62, STORE_REG   = 0x63,

} Opcode;


// Maps
typedef struct {
    char *name;
    unsigned char opNormal;
    unsigned char opReg;
} InstructionMap;

typedef struct {
    char *name;
    int value;
} RegisterMap;

extern const InstructionMap instructionTable[];
extern const RegisterMap registerTable[];


typedef struct {
    char name[256];
    int address;
} LabelSymbol;

int generate(TokenArray tokens, Config cfg);

#endif
