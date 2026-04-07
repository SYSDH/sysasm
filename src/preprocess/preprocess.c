#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "preprocess.h"
#include "../getcode/getcode.h"

#define MAX_INCLUDES 1024

static char includedFiles[MAX_INCLUDES][PATH_MAX];
static int includedCount = 0;

static void getDirectory(const char *path, char *dest) {
    char *lastSlash = strrchr(path, '/');
    char *lastBackslash = strrchr(path, '\\');
    char *sep = (lastSlash > lastBackslash) ? lastSlash : lastBackslash;

    if (sep) {
        size_t len = sep - path;
        strncpy(dest, path, len);
        dest[len] = '\0';
    } else {
        strcpy(dest, ".");
    }
}

static char* processRecursive(const char *filename, Config cfg) {
    logVerbose(cfg, "cyan", "FILE", "preprocessing %s file", filename);

    char absolutePath[PATH_MAX];
#ifdef _WIN32
    _fullpath(absolutePath, filename, PATH_MAX);
#else
    if (realpath(filename, absolutePath) == NULL) {
        strncpy(absolutePath, filename, PATH_MAX);
    }
#endif

    for (int i = 0; i < includedCount; i++) {
        if (strcmp(includedFiles[i], absolutePath) == 0) {
            return strdup("");
        }
    }

    if (includedCount < MAX_INCLUDES) {
        strncpy(includedFiles[includedCount++], absolutePath, PATH_MAX);
    }

    FILE *f = fopen(filename, "r");
    if (!f) {
        showError(FATAL_ERROR, "Could not open include file: %s", filename);
        return NULL;
    }

    char *output = calloc(1, 1);

    if (!output) {showError(FATAL_ERROR, "memory allocation failed"); return NULL;}

    size_t totalSize = 1;

    char line[1024];
    char currentDir[PATH_MAX];
    getDirectory(filename, currentDir);

    while (fgets(line, sizeof(line), f)) {
        char *trimmed = line;
        while (isspace(*trimmed)) trimmed++;

        if (strncmp(trimmed, "#include", 8) == 0) {
            char *startQuote = strchr(trimmed, '"');
            char *endQuote = strrchr(trimmed, '"');

            if (startQuote && endQuote && startQuote != endQuote) {
                char includeName[256];
                size_t nameLen = endQuote - startQuote - 1;
                strncpy(includeName, startQuote + 1, nameLen);
                includeName[nameLen] = '\0';

                size_t pathLen = strlen(currentDir) + strlen(includeName) + 2;
                char *fullIncludePath = malloc(pathLen);

                if (!fullIncludePath) {
                    showError(FATAL_ERROR, "insufficient memory");
                    return NULL;
                }

                snprintf(fullIncludePath, pathLen, "%s/%s", currentDir, includeName);

                char *includedContent = processRecursive(fullIncludePath, cfg);
                
                free(fullIncludePath);
                if (includedContent) {
                    size_t incLen = strlen(includedContent);

                    output = realloc(output, totalSize + incLen + 1);
                    strcat(output, includedContent);
                    strcat(output, "\n");
                    totalSize = strlen(output) + 1;

                    free(includedContent);
                }

                
                continue;
            }
        }

        int insideQuotes = 0;
        
        for (int i = 0; line[i] != '\0'; i++) {
            if (line[i] == '"') {
                insideQuotes = !insideQuotes;
            } else if (!insideQuotes) {
                line[i] = (char)tolower((unsigned char)line[i]);
            }
        }
        
        size_t lineLen = strlen(line);
        char *temp = realloc(output, totalSize + lineLen + 1);

        if (!temp) {
            free(output);
            showError(FATAL_ERROR, "memory allocation failed");
            return NULL;
        }

        output = temp;
        
        strcat(output, line);
        totalSize += lineLen; 
    }

    fclose(f);
    return output;
}

char* preprocessFile(const char *filename, Config cfg) {
    includedCount = 0;
    return processRecursive(filename, cfg);
}