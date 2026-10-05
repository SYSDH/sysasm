#include "args/args.h"

void handleVerbose(const char *val, ArgCtx *ctx) {
    (void)val;

    ctx->verbose = 1;
}