#include "../args.h"

void handleEntryPoint(const char *val, void *context) {
    (void)val;
    
    Config *cfg = (Config *)context;
    cfg->searchEntryPoint = 0;
}