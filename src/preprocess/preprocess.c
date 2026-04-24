#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "preprocess.h"

#define MAX_INCLUDES 1024
#define MAX_MACROS 1024
#define MAX_MACROVALUE 12288
#define MAX_MACRONAME 256
#define MAX_ARGS 128

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

static void expandArgs(const char *template, char **args, int argCount, char *dest, size_t maxDest) {
    size_t dIdx = 0;
    for (size_t i = 0; template[i] != '\0' && dIdx < maxDest - 1; i++) {
        if (template[i] == '%' && isdigit((unsigned char)template[i + 1])) {
            int argIdx = template[i + 1] - '0';
            if (argIdx < argCount && args[argIdx] != NULL) {
                size_t argLen = strlen(args[argIdx]);
                if (dIdx + argLen < maxDest - 1) {
                    memcpy(&dest[dIdx], args[argIdx], argLen);
                    dIdx += argLen;
                }
            }
            i++;

        } else {
            dest[dIdx++] = template[i];
        }
    }
    dest[dIdx] = '\0';
}

static char* applyMacro(const char *line, Config cfg) {
    size_t bufSize = 2048;
    char *result = malloc(bufSize);
    if (!result) return NULL;
    
    size_t destIdx = 0;
    size_t srcIdx = 0;
    int insideQuotes = 0;
    size_t lineLen = strlen(line);

    while (srcIdx < lineLen) {
        if (destIdx >= bufSize - MAX_MACROVALUE - 1) {
            bufSize *= 2;
            char *temp = realloc(result, bufSize);
            if (!temp) { free(result); return NULL; }
            result = temp;
        }

        if (line[srcIdx] == '"') {
            int backslashes = 0;
            int j = (int)(srcIdx) - 1;
            while (j >= 0 && line[j] == '\\') { backslashes++; j--; }
            if (backslashes % 2 == 0) insideQuotes = !insideQuotes;
        }

        if (line[srcIdx] == '%' && !insideQuotes) {
            char nameBuf[MAX_MACRONAME] = {0};
            int n = 0;
            size_t tempIdx = srcIdx + 1;
                        
            while (tempIdx < lineLen && (isalnum((unsigned char)line[tempIdx]) || line[tempIdx] == '_')) {
                if (n < MAX_MACRONAME - 1) nameBuf[n++] = line[tempIdx];
                tempIdx++;
            }
            
            if (n > 0) {
                int foundIdx = -1;
                for (int i = 0; i < macroCount; i++) {
                    if (strcmp(macros[i].name, nameBuf) == 0) {
                        foundIdx = i;
                        break;
                    }
                }

                if (foundIdx != -1) {
                    char *args[MAX_ARGS] = {0};
                    int argCount = 0;
                    size_t argPtr = tempIdx;

                    int expectedArgs = 0;

                    for (int i = 0; macros[foundIdx].value[i] != '\0'; i++) {
                        if (macros[foundIdx].value[i] == '%' && isdigit((unsigned char)macros[foundIdx].value[i+1])) {
                            int idx = macros[foundIdx].value[i+1] - '0';
                            if (idx >= expectedArgs) expectedArgs = idx + 1;
                        }
                    }
                    
                    if (expectedArgs > 0) {
                        while (argPtr < lineLen && isspace((unsigned char)line[argPtr])) {
                            if (line[argPtr] == '\n' || line[argPtr] == '\r') break;
                            argPtr++;
                        }

                        while (argPtr < lineLen && argCount < expectedArgs) {
                            if (line[argPtr] == '\n' || line[argPtr] == '\r' || line[argPtr] == ';') break;

                            int start = argPtr;
                            while (argPtr < lineLen && !isspace((unsigned char)line[argPtr]) && line[argPtr] != ',' && line[argPtr] != ';') {
                                argPtr++;
                            }
                            
                            int len = argPtr - start;
                            if (len > 0) {
                                args[argCount] = malloc(len + 1);
                                if (args[argCount]) {
                                    strncpy(args[argCount], &line[start], len);
                                    args[argCount][len] = '\0';
                                    argCount++;
                                }
                            }

                            while (argPtr < lineLen && (isspace((unsigned char)line[argPtr]) || line[argPtr] == ',')) {
                                if (line[argPtr] == '\n' || line[argPtr] == '\r') break;
                                argPtr++;
                            }
                        }
                    }

                    char expanded[MAX_MACROVALUE];
                    expandArgs(macros[foundIdx].value, args, argCount, expanded, MAX_MACROVALUE);

                    logVerbose(cfg, "magenta", "MACRO", "Expanded '%%%s' (%d args) -> '%s'", nameBuf, argCount, expanded);

                    char leadingWhitespaces[256] = {0};
                    int lw = 0;
                    int back = (int)destIdx - 1;

                    while (back >= 0 && (result[back] == ' ' || result[back] == '\t')) {
                        back--;
                    }

                    if (back < 0 || result[back] == '\n') { 
                        lw = (int)destIdx - 1 - back;
                        if (lw > 255) lw = 255;
                        for (int j = 0; j < lw; j++) leadingWhitespaces[j] = result[back + 1 + j];
                    }
                    
                    size_t expLen = strlen(expanded);
                    int newlines = 0;
                    for (size_t i = 0; i < expLen; i++) if (expanded[i] == '\n') newlines++;

                    if (destIdx + expLen + (newlines * lw) + 512 >= bufSize) {
                        bufSize += expLen + (newlines * lw) + 1024;
                        char *temp = realloc(result, bufSize);
                        if (!temp) { 
                            for (int i = 0; i < argCount; i++) free(args[i]);
                            free(result); return NULL; 
                        }
                        result = temp;
                    }

                    for (size_t i = 0; i < expLen; i++) {
                        result[destIdx++] = expanded[i];
                        if (expanded[i] == '\n' && expanded[i+1] != '\0') {
                            for (int j = 0; j < lw; j++) {
                                result[destIdx++] = leadingWhitespaces[j];
                            }
                        }
                    }

                    srcIdx = argPtr; 

                    for (int i = 0; i < argCount; i++) free(args[i]);
                    continue;
                }
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

    int insideQuotes = 0;
    
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
            char macroName[MAX_MACRONAME] = {0};
            char macroVal[MAX_MACROVALUE] = {0};

            sscanf(trimmed, "%%macro %s", macroName);

            char *openBrace = strchr(trimmed, '{');
            if (openBrace) {
                char lineBuffer[1024];
                int firstLine = 1;
                int miLen = 0;
                
                while (fgets(lineBuffer, sizeof(lineBuffer), f)) {
                    char *closeBrace = strchr(lineBuffer, '}');
                    char *start = lineBuffer;

                    if (firstLine) {
                        int tempLen = 0;
                        while (start[tempLen] == ' ' || start[tempLen] == '\t') tempLen++;
                        
                        if (start[tempLen] == '\n' || start[tempLen] == '\r') {
                            continue;
                        } else if (start[tempLen] == '}') {
                            break;
                        } else {
                            miLen = tempLen;
                            firstLine = 0;
                            start += miLen;
                        }
                    } else {
                        int stripped = 0;
                        
                        while (stripped < miLen && (*start == ' ' || *start == '\t')) {
                            stripped++;
                            start++;
                        }
                    }

                    if (closeBrace) {
                        char *cb = strchr(start, '}');
                        if (cb) *cb = '\0';
                        strncat(macroVal, start, sizeof(macroVal) - strlen(macroVal) - 1);
                        break;
                    }
                    strncat(macroVal, start, sizeof(macroVal) - strlen(macroVal) - 1);
                }
            } else {
                char *val = strstr(trimmed, macroName) + strlen(macroName);
                while(isspace(*val)) val++;
                strncpy(macroVal, val, MAX_MACROVALUE - 1);
                macroVal[MAX_MACROVALUE - 1] = '\0';

                int len = strlen(macroVal);
                while (len > 0 && isspace((unsigned char)macroVal[len - 1])) {
                    macroVal[len - 1] = '\0';
                    len--;
                }
            }

            if (macroCount < MAX_MACROS) {
                for(int k = 0; macroName[k]; k++) macroName[k] = (char)tolower((unsigned char)macroName[k]);

                strncpy(macros[macroCount].name, macroName, MAX_MACRONAME);
                strncpy(macros[macroCount].value, macroVal, MAX_MACROVALUE);

                logVerbose(cfg, "magenta", "MACRO", "Defined macro '%s' ->  '%s'", macroName, macroVal);
                macroCount++;
            }
            continue;
        }

        
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
        int depth = 0;
        
        while (depth < 20) {

            char *nextPass = applyMacro(processedLine, cfg);
            if (!nextPass) break;
            if (strcmp(nextPass, processedLine) == 0) {
                free(nextPass);
                break;
            }
            free(processedLine);
            processedLine = nextPass;
            depth++;
        }

        if (processedLine) {
            size_t finalLen = strlen(processedLine);
            char *temp = realloc(output, totalSize + finalLen + 1);

            if (!temp) {
                free(output);
                free(processedLine);
                showError(FATAL_ERROR, "Memory allocation failed");
                return NULL;
            }
            output = temp;
            
            memcpy(output + totalSize - 1, processedLine, finalLen + 1);
            totalSize += finalLen;
            
            free(processedLine);
        }
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