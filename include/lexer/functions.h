#pragma once

#include "structs.h"
#include "kind.h"
#include "args/args.h"

void freeTokens(TokenArray *tokens);

void tokenize(const char *code, TokenArray *tokens, ArgCtx *ctx);

void addTok(
    Lexer *lexer,
    TokenKind kind,
    const char *value,
    int len,
    int line,
    int col
);

void showTokens(const TokenArray *tokens);


