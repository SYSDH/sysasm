#include "../args.h"

void handleEntryPoint(const char *val, void *context) {
    Config *cfg = (Config *)context;
    cfg->searchEntryPoint = 0;
}