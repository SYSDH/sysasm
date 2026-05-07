#ifndef HELPERS_H
#define HELPERS_H

#include <stddef.h>
#include "../args/args.h"

typedef enum {
    WARNING_ERROR = 0,
    FATAL_ERROR = 1
} Severity;

extern char *programName;

int contains(const char *arr[], size_t n, const char *value);

void showError(Severity sev, const char *extraMessage, ...);
void setProgram(char *programVar);
void logVerbose(Config cfg, const char *color, const char *step, const char *fmt, ...);
void fixUtf();

#endif
