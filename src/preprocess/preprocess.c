#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "preprocess.h"
#include "../getcode/getcode.h"

#define MAX_INCLUDES 1024
#define MAX_MACROS 1024
#define MAX_MACRONAME 256
#define MAX_MACROVALUE 256

static char includedFiles[MAX_INCLUDES][PATH_MAX];
static int includedCount = 0;

typedef struct {
    char name[MAX_MACRONAME];
    char value[MAX_MACROVALUE];
} Macro;

static Macro macros[MAX_MACROS];
static int macroCount = 0;

static void getDirectory(const char *path, char *dest) {
    char *lastSlash = strrchr(path, '/');
    char *lastBackslash = strrchr(path, '\\');
    char *sep = (lastSlash > lastBackslash) ? lastSlash : lastBackslash;

    if (sep) {
        size_t len = sep - path;
        if (len >= PATH_MAX) len = PATH_MAX - 1;

        strncpy(dest, path, len);
        dest[len] = '\0';
    } else {
        strcpy(dest, ".");
    }
}

static char* applyMacro(const char *line, Config cfg) {
    size_t bufSize = 2048;
    char *result = malloc(bufSize);
    if (!result) return NULL;
    
    size_t destIdx = 0;
    size_t srcIdx = 0;
    int insideQuotes = 0;

    while (!(line[srcIdx] == '\0')) {
        if (destIdx >= bufSize - MAX_MACROVALUE - 1) {
            bufSize *= 2;
            
            char *temp = realloc(result, bufSize);
            if (!temp) { free(result); return NULL; }
            result = temp;
        }

        if (line[srcIdx] == '"') {
            int backslashes = 0;
            int j = (int)srcIdx - 1;
            while (j >= 0 && line[j] == '\\') { backslashes++; j--; }
            if (backslashes % 2 == 0) insideQuotes = !insideQuotes;
        }

        if (line[srcIdx] == '%' && !insideQuotes) {
            char nameBuf[MAX_MACRONAME] = {0};
            int n = 0;
            size_t tempSrc = srcIdx + 1;
            
            while (isalnum((unsigned char)line[tempSrc]) || line[tempSrc] == '_') {
                if (n < MAX_MACRONAME - 1) {
                    nameBuf[n++] = line[tempSrc];
                }
                tempSrc++;
            }
            
            if (n > 0) {
                int found = 0;
                for (int i = 0; i < macroCount; i++) {
                    if (strcmp(macros[i].name, nameBuf) == 0) {
                        size_t valLen = strlen(macros[i].value);

                        logVerbose(cfg, "magenta", "MACRO", "Expanded '%%%s' -> '%s'", nameBuf, macros[i].value);

                        memcpy(&result[destIdx], macros[i].value, valLen);
                        destIdx += valLen;
                        srcIdx = tempSrc;
                        found = 1;

                        break;
                    }
                }
                if (found) continue;
            }
        }
        
        result[destIdx++] = line[srcIdx++];
    }
    
    result[destIdx] = '\0';
    return result;
}

static char* processRecursive(const char *filename, Config cfg) {
    logVerbose(cfg, "cyan", "PREPROCESS", "Preprocessing %s file", filename);

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
            logVerbose(cfg, "yellow", "INCLUDE", "Circular dependency detected for %s, skipping.", filename);
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

        if (!strncmp(trimmed, "#include", 8)) {
            char *startQuote = strchr(trimmed, '"');
            char *endQuote = strrchr(trimmed, '"');

            if (startQuote && endQuote && startQuote != endQuote) {
                char includeName[256];
                size_t nameLen = endQuote - startQuote - 1;
                strncpy(includeName, startQuote + 1, nameLen);
                includeName[nameLen] = '\0';

                logVerbose(cfg, "yellow", "INCLUDE", "Found '#include \"%s\"'", includeName);

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

        if (!strncmp(trimmed, "%macro", 6)) {
            char macroName[MAX_MACRONAME];
            char macroVal[MAX_MACROVALUE];
            
            if (sscanf(trimmed, "%%macro %255s %255[^\n\r]", macroName, macroVal) == 2) {
                for(int k = 0; macroName[k]; k++) { macroName[k] = tolower(macroName[k]);}
                
                int valLen = strlen(macroVal);
                while(valLen > 0 && isspace((unsigned char)macroVal[valLen - 1])) {
                    macroVal[--valLen] = '\0';
                }

                if (macroCount < MAX_MACROS) {
                    strncpy(macros[macroCount].name, macroName, MAX_MACRONAME);
                    strncpy(macros[macroCount].value, macroVal, MAX_MACROVALUE);

                    logVerbose(cfg, "magenta", "MACRO", "Defined macro '%s' -> '%s'", macroName, macroVal);
                    
                    macroCount++;
                }
            }
            continue;

        }

        int insideQuotes = 0;

        for (int i = 0; line[i] != '\0'; i++) {
            if (line[i] == '"') {
                int backslashes = 0;
                int j = i - 1;

                while (j >= 0 && line[j] == '\\') {
                    backslashes++;
                    j--;
                }

                if (backslashes % 2 == 0) {
                    insideQuotes = !insideQuotes;
                }
            } else if (!insideQuotes) {
                line[i] = (char)tolower((unsigned char)line[i]);
            }
        }
                
        char *processedLine = applyMacro(line, cfg);
        if (!processedLine) {
            free(output);
            showError(FATAL_ERROR, "memory allocation failed in macro expansion");
            return NULL;
        }

        size_t lineLen = strlen(processedLine);
        char *temp = realloc(output, totalSize + lineLen + 1);

        if (!temp) {
            free(output);
            free(processedLine);
            showError(FATAL_ERROR, "memory allocation failed");
            return NULL;
        }

        output = temp;
        strcat(output, processedLine);
        totalSize += lineLen; 
        
        free(processedLine);
    }
    

    fclose(f);
    return output;
}

char* preprocessFile(const char *filename, Config cfg) {
    includedCount = 0;
    macroCount = 0;

    
    logVerbose(cfg, "cyan", "PREPROCESS", "Starting preprocessing session...");
    
    char* result = processRecursive(filename, cfg);

    logVerbose(cfg, "cyan", "PREPROCESS", "Preprocessing completed successfully.");

    return result;
}