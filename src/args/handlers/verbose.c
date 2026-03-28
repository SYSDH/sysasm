#include "../args.h"

void handleVerbose(const char *val, void *context) {
    Config *cfg = (Config *)context;
    cfg->verbose = 1;
}