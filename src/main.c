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

    ArgCtx ctx = {
        .outputName       = "out.bin", 
        .searchEntryPoint = 1, 
        .verbose          = 0, 
        .pos              = NULL,
    };

    if (parseArgv(argc, argv, &ctx)) return 1;
    if (!ctx.pos) { showError(FATAL_ERROR, "no input files"); return 1; }

    logVerbose(ctx, "cyan", "PREPROCESS", "Reading %s file", ctx.pos);

    char *code = preprocessFile(ctx.pos, ctx);
    if (!code) return 1;

    TokenArray tokens;

    tokens.size     = 0;
    tokens.capacity = 10;
    tokens.data     = malloc(tokens.capacity * sizeof(Token));

    if (!tokens.data) { 
        showError(FATAL_ERROR, "error to allocate memory to tokens.data");
        return 1;
    }
 
    logVerbose(ctx, "green", "LEXER", "Start Tokenize step");
    tokenize(code, &tokens, &ctx);

    logVerbose(ctx, "magenta", "GENERATE", "Start Generate code step");
    
    int ret = gen(&tokens, &ctx);

    free(code);
    free(tokens.data);
    
    return ret;
}