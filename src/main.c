#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/lexer.h"
#include "preprocess/preprocess.h"
#include "gen/gen.h"
#include "args/args.h"
#include "utils.h"

int main(int argc, char **argv) {
    fixUtf();
    
    setProgram(argv[0]);

    Config cfg = {"out.bin", 1, 0};
    char *pos = NULL;

    if (parseArgs(argc, argv, &cfg, &pos)) return 1;
    if (!pos) { showError(FATAL_ERROR, "no input files"); return 1;}

    logVerbose(cfg, "cyan", "PREPROCESS", "Reading %s file", pos);

    char *code = preprocessFile(pos, cfg);

    if (!code) {
        return 1;
    }

    TokenArray tokens;

    tokens.size = 0;
    tokens.capacity = 10;
    tokens.data = malloc(tokens.capacity * sizeof(Token));

    if (!tokens.data) { 
        showError(FATAL_ERROR, "error to allocate memory to tokens.data");
        return 1;
    }
 
    logVerbose(cfg, "green", "LEXER", "Start Tokenize step");
    tokenize(code, &tokens, cfg);

    logVerbose(cfg, "magenta", "GENERATE", "Start Generate code step");
    
    int ret = generate(tokens, cfg);

    free(code);
    free(tokens.data);
    
    return ret;
}