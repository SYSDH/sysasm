#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../args.h"

void handleVersion(const char *val, void *context) {
    const char *name = "SYSDH Software Suite\nHasm: ";
    const char *version = "0.4.6-beta";


    printf("%s%s\n", name, version);
    exit(0);
}