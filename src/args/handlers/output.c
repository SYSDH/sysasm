#include "../args.h"

void handleOutput(const char *val, void *context) {
    Config *cfg = (Config *)context;
    cfg->outputName = (char *)val;
}