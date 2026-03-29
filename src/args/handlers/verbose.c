#include "../args.h"

void handleVerbose(const char *val, void *context) {
    (void)val;

    Config *cfg = (Config *)context;
    cfg->verbose = 1;
}