#pragma once

#include "table.h"

#define X(type, str        ) type,
#define Y(type, str, search)
#define Z(type, str, search) type,

typedef enum TokenKind {
    TOKENS_TABLE TOKEN_KEYWORD
} TokenKind;

#undef X
#undef Y
#undef Z

