#include <stdio.h>
#include <string.h>

#include "args.h"
#include "../helpers/helpers.h"

ArgOption options[] = {
    {"v", "version", "Show the version of project", ARG_NONE, handleVersion},
    {"o", "output", "Set output name", ARG_REQ, handleOutput},
    {"h", "help", "Show this message", ARG_OPT, handleHelp},

    {NULL, "search-entry", "Skip Search entry point from code", ARG_NONE, handleEntryPoint},
    {"V", "verbose", "Run code in verbose mode, sampling the compilation phase", ARG_NONE, handleVerbose},
};

const int optCount = sizeof(options) / sizeof(options[0]);

int parseArgs(int argc, char **argv, void *context, char **targetPos) {
    for (int i = 1; i < argc; i++) {
        char *current = argv[i];

        if (current[0] != '-' || current[1] == '\0') {

            if (targetPos != NULL) {
                *targetPos = current;
            }
            continue;
        }

        int isLong = (current[1] == '-');
        int found = 0;

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
                }
                else if (options[j].hasVal == ARG_OPT) {
                    if (i + 1 < argc && argv[i + 1][0] != '-') {
                        val = argv[++i];
                    }
                }

                options[j].handler(val, context);
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
