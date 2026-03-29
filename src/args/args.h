#ifndef ARGS_H
#define ARGS_H

#define ARG_NONE 0
#define ARG_REQ 1
#define ARG_OPT 2

typedef struct {
    const char *shortOpt;
    const char *longOpt;
    const char *desc;

    int hasVal;
    void (*handler)(const char *val, void *context);
} ArgOption;

typedef struct {
    char *outputName;
    int searchEntryPoint;
    int verbose;
} Config;

extern ArgOption options[];
extern const int optCount;

int parseArgs(int argc, char **argv, void *context, char **targetPos);

void handleVersion(const char *val, void *context);
void handleOutput(const char *val, void *context);
void handleHelp(const char *val, void *context);
void handleEntryPoint(const char *val, void *context);
void handleVerbose(const char *val, void *context);

#endif
