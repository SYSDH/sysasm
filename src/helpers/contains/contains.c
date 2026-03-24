#include <string.h>
#include <stddef.h>

#include "../helpers.h"

int contains(const char *arr[], size_t n, const char *value) {
    for (size_t i = 0; i < n; i++)
        if (strcmp(arr[i], value) == 0) {
            return 1;
        }
    return 0;
}