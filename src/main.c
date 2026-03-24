#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/lexer.h"
#include "preprocess/preprocess.h"
#include "generate/generate.h"
#include "getCode/getCode.h"
int main(int argc, char **argv) {
    char *code = getCode(argv[1]);

    preprocess(code);

    TokenArray tokens;

    tokens.size = 0;
    tokens.capacity = 10;
    tokens.data = malloc(tokens.capacity * sizeof(Token));

    if (!tokens.data) { 
        printf("Error to allocate memory\n");
        return 1;
    }
    
    tokenize(code, &tokens);
    
    return generate(tokens);
}