#pragma once

#include <stddef.h>
#include <stdint.h>

#include "structs.h"
#include "functions.h"
#include "tokenize.h"

#include "kind.h"

#define INIT_TOKENS(identifier)                                    \
    TokenArray tokens = {.size = 0, .capacity = 10, .data = NULL}; \
    tokens.data = malloc(tokens.capacity * sizeof *tokens.data);


