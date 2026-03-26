#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/lexer.h"
#include "preprocess/preprocess.h"
#include "generate/generate.h"
#include "getCode/getCode.h"
#include "args/args.h"
#include "helpers/helpers.h"

int main(int argc, char **argv) {
    setProgram(argv[0]);

    Config cfg = {"out.bin"};
    char *pos = NULL;

    if (parseArgs(argc, argv, &cfg, &pos)) return 1;
    if (!pos) { showError(FATAL_ERROR, "no input files"); return 1;}

    char *code = getCode(pos);

    preprocess(code);

    TokenArray tokens;

    tokens.size = 0;
    tokens.capacity = 10;
    tokens.data = malloc(tokens.capacity * sizeof(Token));

    if (!tokens.data) { 
        showError(FATAL_ERROR, "error to allocate memory to tokens.data");
        return 1;
    }
    
    tokenize(code, &tokens);

    int ret = generate(tokens, cfg);

    free(code);
    free(tokens.data);
    
    return ret;
}