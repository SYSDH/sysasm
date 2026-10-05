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

    logVerbose(&ctx, "cyan", "PREPROCESS", "Reading %s file", ctx.pos);

    char *code = preprocessFile(ctx.pos, &ctx);
    if (!code) return 1;

    int ret = 1;

    INIT_TOKENS(tokens);

    if (!tokens.data) { 
        showError(FATAL_ERROR, "error to allocate memory to tokens.data");
        goto cleanUp;
    }
 
    logVerbose(&ctx, "green", "LEXER", "Start Tokenize step");
    tokenize(code, &tokens, &ctx);

    showTokens(&tokens);

    logVerbose(&ctx, "magenta", "GENERATE", "Start Generate code step");
    
    if (gen(&tokens, &ctx)) goto cleanUp;

    ret = 0;

cleanUp:
    if (code)        free(code);
    if (tokens.data) freeTokens(&tokens);
    
    return ret;
}