#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "../args.h"
#include "../../helpers/helpers.h"

void handleHelp(const char *val, void *context) {
    #define BRIGHT "\x1b[1m"
    #define RESET "\x1b[0m"

    (void)context;

    if (val) {
        char cpyval[256];
        int i;
        int found = -1;

        for (i = 0; i < (int)sizeof(cpyval) - 1 && val[i] != '\0'; i++) {
            cpyval[i] = tolower((unsigned char)val[i]);
        }

        cpyval[i] = '\0';

        for (int i = 0; i < optCount; i++) {
            if (strcmp(options[i].longOpt, cpyval) == 0) {
                found = i;
                break;
            }
        }
        if (found != -1) {
            printf("%s\n", options[found].desc);
            exit(0);
        }
        char buff[256];
        snprintf(buff, 256, "unknown command-line option '%s', showing general help:", val);

        showError(WARNING_ERROR, buff);
    }

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