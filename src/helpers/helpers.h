#ifndef HELPERS_H
#define HELPERS_H

#include <stddef.h>

typedef enum {
    WARNING_ERROR = 0,
    FATAL_ERROR = 1
} Severity;

extern char *programName;

int contains(const char *arr[], size_t n, const char *value);

char *findValue(const char *str);

void showError(Severity sev, char *extraMessage);
void setProgram(char *programVar);

#endif
