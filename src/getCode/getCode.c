#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <limits.h>

#include "../helpers/helpers.h"

char* getCode(char *fileName) {
    FILE *f = fopen(fileName, "rb");
    if (!f) {
        showError(FATAL_ERROR, "file: \"%s\" not found", fileName);
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *content = malloc(size + 1);
    if (!content) {
        showError(FATAL_ERROR, "error do allocate memory");
        fclose(f);
        exit(1);
    }

    size_t actualRead = fread(content, 1, size, f);
    content[actualRead] = '\0';

    fclose(f);

    return content;
}