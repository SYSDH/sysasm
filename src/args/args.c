#include <stdio.h>
#include <string.h>

#include "args/args.h"
#include "utils.h"

#define X(short, long, desc, argType, handler) {short, long, desc, argType, handler},
ArgOption options[] = {
    ARGS_TABLE
};
#undef X

const int optCount = sizeof(options) / sizeof(options[0]);

int parseArgv(int argc, char **argv, ArgCtx *ctx) {
    for (int i = 1; i < argc; i++) {
        char *current = argv[i];

        if (current[0] != '-' || current[1] == '\0') {
            if (ctx->pos == NULL) {
                ctx->pos = current;
            }

            continue;
        }

        int isLong = (current[1] == '-');
        int found  = 0;

        for (int j = 0; j < optCount; j++) {
            const char *target = isLong ? options[j].longOpt : options[j].shortOpt;
            
            if (target != NULL && strcmp(current + (isLong ? 2 : 1), target) == 0) {
                char *val = NULL;

                if (options[j].hasVal == ARG_REQ) {
                    if (i + 1 < argc) {
                        val = argv[++i];
                    } else {
                        showError(FATAL_ERROR, "the option %s need a value.", current);
                        return 1;
                    }
                } else if (options[j].hasVal == ARG_OPT) {
                    if (i + 1 < argc && argv[i + 1][0] != '-') {
                        val = argv[++i];
                    }
                }

                options[j].handler(val, ctx);
                found = 1;

                break;
            }
        }

        if (!found) {
            showError(FATAL_ERROR, "unrecognized command-line option '%s'", current);
            return 1;
        }
    }

    return 0;
}
