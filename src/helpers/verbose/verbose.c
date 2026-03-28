#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "../../args/args.h"

typedef struct {
    const char *name;
    const char *code;
} Color;

Color colors[] = {
    {"black",   "\033[30m"},
    {"red",     "\033[31m"},
    {"green",   "\033[32m"},
    {"yellow",  "\033[33m"},
    {"blue",    "\033[34m"},
    {"magenta", "\033[35m"},
    {"cyan",    "\033[36m"},
    {"white",   "\033[37m"},
    {"reset",   "\033[0m"}
};

void setColor(const char *colorName) {
    for (int i = 0; i < sizeof(colors)/sizeof(Color); i++) {
        if (strcmp(colorName, colors[i].name) == 0) {
            printf("\033[1m%s", colors[i].code);
            return;
        }
    }
    printf("\033[0m");
}

void logVerbose(Config cfg, const char *color, const char *step, const char *fmt, ...) {
    if (!cfg.verbose) return;

    va_list args;
    va_start(args, fmt);
    
    setColor(color);
    printf("[%s] ", step);
    setColor("reset");
    
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}