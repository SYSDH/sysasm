#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <limits.h>

#include "../helpers/helpers.h"

char* getCode(char *fileName) {
    FILE *f = fopen(fileName, "rb");
    if (!f) {

        printf("file: \"%s\" not found\n", fileName);
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *content = malloc(size + 1);
    if (!content) {
        printf("Malloc error");
        fclose(f);
        exit(1);
    }

    size_t actualRead = fread(content, 1, size, f);
    content[actualRead] = '\0';

    fclose(f);

    return content;
}