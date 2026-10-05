#pragma once

/*
X = literals
Y = keywords/types
Z = operators/symbols
*/


// ============================================================
// TOKEN TABLES
// ============================================================

// -------------------- Keywords ------------------------------

#define TOKENS_KEYWORDS_TABLE                  \
    Y(TOKEN_KEYWORD, "EXIT(%s)",      "exit" ) \
    Y(TOKEN_KEYWORD, "MOV(%s)",       "mov"  ) \
    Y(TOKEN_KEYWORD, "ADD(%s)",       "add"  ) \
    Y(TOKEN_KEYWORD, "SUB(%s)",       "sub"  ) \
    Y(TOKEN_KEYWORD, "JZ(%s)",        "jz"   ) \
    Y(TOKEN_KEYWORD, "JNZ(%s)",       "jnz"  ) \
    Y(TOKEN_KEYWORD, "JMP(%s)",       "jmp"  ) \
    Y(TOKEN_KEYWORD, "CALL(%s)",      "call" ) \
    Y(TOKEN_KEYWORD, "RET(%s)",       "ret"  ) \
    Y(TOKEN_KEYWORD, "JG(%s)",        "jg"   ) \
    Y(TOKEN_KEYWORD, "JGE(%s)",       "jge"  ) \
    Y(TOKEN_KEYWORD, "JL(%s)",        "jl"   ) \
    Y(TOKEN_KEYWORD, "JLE(%s)",       "jle"  ) \
    Y(TOKEN_KEYWORD, "OUT(%s)",       "out"  ) \
    Y(TOKEN_KEYWORD, "IN(%s)",        "in"   ) \
    Y(TOKEN_KEYWORD, "LOADF(%s)",     "loadf") \
    Y(TOKEN_KEYWORD, "PUSH(%s)",      "push" ) \
    Y(TOKEN_KEYWORD, "POP(%s)",       "pop"  ) \
    Y(TOKEN_KEYWORD, "LOAD(%s)",      "load" ) \
    Y(TOKEN_KEYWORD, "STORE(%s)",     "store") \
    Y(TOKEN_KEYWORD, "DB(%s)",        "db"   ) \
    \
    Y(TOKEN_KEYWORD, "H(%s)",  "h" ) \
    Y(TOKEN_KEYWORD, "HE(%s)", "he") \
    Y(TOKEN_KEYWORD, "LI(%s)", "li") \
    Y(TOKEN_KEYWORD, "BE(%s)", "be") \
    Y(TOKEN_KEYWORD, "B(%s)",  "b" ) \
    Y(TOKEN_KEYWORD, "C(%s)",  "c" ) \
    Y(TOKEN_KEYWORD, "N(%s)",  "n" ) \
    Y(TOKEN_KEYWORD, "O(%s)",  "o" ) \
    Y(TOKEN_KEYWORD, "SP(%s)", "sp") \
    Y(TOKEN_KEYWORD, "BP(%s)", "bp") \

// -------------------- Operators ----------------------------

#define TOKENS_OPERATORS_TABLE              \
    Z(TOKEN_PLUS,     "PLUS(%s)",     "+" ) \
    Z(TOKEN_MINUS,    "MINUS(%s)",    "-" ) \
    Z(TOKEN_MULTIPLY, "MULTIPLY(%s)", "*" ) \
    Z(TOKEN_DIVIDE,   "DIVIDE(%s)",   "/" ) \
    Z(TOKEN_BIT_AND,  "BIT_AND(%s)",  "&" ) \
    Z(TOKEN_BIT_OR,   "BIT_OR(%s)",   "|" ) \
    Z(TOKEN_BIT_XOR,  "BIT_XOR(%s)",  "^" ) \
    Z(TOKEN_BIT_NOT,  "BIT_NOT(%s)",  "~" ) \


// -------------------- Symbols -------------------------------

#define TOKENS_SYMBOLS_TABLE               \
    Z(TOKEN_ASSIGN,   "ASSIGN(%s)",   "=") \
    Z(TOKEN_COMMA,    "COMMA(%s)",    ",") \
    Z(TOKEN_POINTER,  "POINTER(%s)",  "$") \
    \
    Z(TOKEN_LPAREN,   "LPAREN(%s)",   "(") \
    Z(TOKEN_RPAREN,   "RPAREN(%s)",   ")") \
    \
    Z(TOKEN_LBRACKET, "LBRACKET(%s)", "[") \
    Z(TOKEN_RBRACKET, "RBRACKET(%s)", "]") \
    \
    Z(TOKEN_LBRACE,   "LBRACE(%s)",   "{") \
    Z(TOKEN_RBRACE,   "RBRACE(%s)",   "}")


// -------------------- Literals ------------------------------

#define TOKENS_LITERALS_TABLE                        \
    X(TOKEN_IDENTIFIER,    "IDENTIFIER_LITERAL(%s)") \
    X(TOKEN_STRING,        "STRING_LITERAL(%s)"    ) \
    X(TOKEN_NUMBER,        "NUMBER(%s)"            ) \
    X(TOKEN_LABEL_DEF,     "TOKEN_LABEL_DEF(%s)"   ) \
    X(TOKEN_LABEL_REF,     "TOKEN_LABEL_REF(%s)"   ) \
    X(TOKEN_DIRECTIVE,     "TOKEN_DIRECTIVE(%s)"   ) \
    X(TOKEN_UNDEFINED,     "UNDEFINED_LITERAL(%s)" ) \

// -------------------- Complete table ------------------------

#define TOKENS_TABLE              \
    TOKENS_KEYWORDS_TABLE         \
    TOKENS_SYMBOLS_TABLE          \
    TOKENS_OPERATORS_TABLE        \
    TOKENS_LITERALS_TABLE

