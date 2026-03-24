#include <stdlib.h>
#include <string.h>

#include "../helpers.h"
#include "../../generate/generate.h"

char *findValue(const char *str) {
    for (int i = 0; i < tableSize; i++) {
        if (strcmp(str, keywordTable[i].name) == 0) {
            return keywordTable[i].value;
        }
    }
    return NULL;
}
