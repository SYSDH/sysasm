#include <stdio.h>
#include <string.h>

#include "args.h"
#include "../helpers/helpers.h"

ArgOption options[] = {
    {"v", "version", "Show the version of project", 1, handleVersion},
    {"o", "output", "Set output name", 1, handleOutput},
    {"h", "help", "Show this message", 0, handleHelp}
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

                if (options[j].hasVal) {

                    if (i + 1 < argc) {
                        val = argv[++i];
                    } else {
                        char buff[256];

                        sprintf(buff, "the option %s need a value.\n", current);
                        showError(FATAL_ERROR, buff);
                        return 1;
                    }
                }

                options[j].handler(val, context);
                found = 1;
                break;
            }
        }

        if (!found) {
            char buff[1024];

            sprintf(buff, "unrecognized command-line option '%s'\n", current);
            showError(FATAL_ERROR, buff);
            return 1;
        }
    }
    return 0;
}
