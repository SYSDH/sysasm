#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "../helpers.h"

char *programName = "";

void showError(Severity sev, const char *extraMessage, ...) {
    va_list msg;
    va_start(msg, extraMessage); 

    #define MAGENTA "\x1b[35m"
    #define RED "\x1b[31m"
    #define RESET "\x1b[0m"

    const char *color = "";
    const char *errorMessage = "";

    if (sev == FATAL_ERROR) {
        errorMessage = "fatal error: ";
        color = RED;
    } else if (sev == WARNING_ERROR) {
        errorMessage = "warning: ";
        color = MAGENTA;
    }

    printf("%s: %s%s%s", programName, color, errorMessage, RESET);
    vprintf(extraMessage, msg);

    printf("\n");

    va_end(msg);
}

void setProgram(char *programVar) {
    char *lastSlash = strrchr(programVar, '/');
    char *lastBackslash = strrchr(programVar, '\\');
    
    char *filename = programVar;

    if (lastSlash && lastBackslash) {
        filename = (lastSlash > lastBackslash) ? lastSlash + 1 : lastBackslash + 1;
    } else if (lastSlash) {
        filename = lastSlash + 1;
    } else if (lastBackslash) {
        filename = lastBackslash + 1;
    }

    programName = filename;
}