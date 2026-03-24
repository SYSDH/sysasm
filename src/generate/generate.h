#ifndef GENERATE_H
#define GENERATE_H

#include <stddef.h>

#include "../lexer/lexer.h"
#include "../args/args.h"

// Commands
#define MOV   "0x10"
#define ADD   "0x20"
#define SUB   "0x21"
#define JZ    "0x30"
#define JNZ   "0x31"
#define JMP   "0x32"
#define WRITE "0x40"
#define EXIT  "0xFF"

// Registers
#define H  "0x0"
#define He "0x1"
#define Li "0x2"
#define Be "0x3"
#define B  "0x4"
#define C  "0x5"
#define N  "0x6"
#define O  "0x7"

typedef struct {
    char *name;
    char *value;
} keywordsStruct;

typedef struct {
    char name[256];
    int address;
} LabelSymbol;

extern const keywordsStruct keywordTable[];
extern const int tableSize;

int generate(TokenArray tokens, Config cfg);

#endif
