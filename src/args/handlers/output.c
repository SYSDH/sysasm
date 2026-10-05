#include "args/args.h"

void handleOutput(const char *val, ArgCtx *ctx) {
    ctx->outputName = (char *)val;
}