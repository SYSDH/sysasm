#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../args.h"
#include "../../helpers/helpers.h"

void handleHelp(const char *val, void *context) {
    #define BRIGHT "\x1b[1m"
    #define RESET "\x1b[0m"

    printf("Usage: %s%s [options] <File>%s\n\n", BRIGHT, programName, RESET);
    printf("%sOptions%s:\n", BRIGHT, RESET);

    for (int i = 0; i < optCount; i++) {
        if (options[i].shortOpt) {
            printf("  -%s%-10s%s --%s%-17s%s %s\n",
                BRIGHT,
                options[i].shortOpt,
                RESET,
                BRIGHT,
                options[i].longOpt,
                RESET,
                options[i].desc
            );
        }
    }
    printf("\n");

    for (int i = 0; i < optCount; i++) {
        if (!options[i].shortOpt) {
            printf("  --%s%-29s%s %s\n",
                BRIGHT,
                options[i].longOpt,
                RESET,
                options[i].desc
            );
        }
    }

    printf("\nExample:\n  %s%s code.hasm -o myprogram.bin %s\n", BRIGHT, programName, RESET);
    
    exit(0);
}