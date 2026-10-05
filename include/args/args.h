#pragma once

#define ARG_NONE 0
#define ARG_REQ 1
#define ARG_OPT 2

//     {"o", "output", "Set output name", ARG_REQ, handleOutput},
//     {"h", "help", "Show this message", ARG_OPT, handleHelp},

//     {NULL, "search-entry", "Skip Search entry point from code", ARG_NONE, handleEntryPoint},
//     {"V", "verbose", "Run code in verbose mode, sampling the compilation phase", ARG_NONE, handleVerbose},
// };

#define ARGS_TABLE \
    X("o", "output", "Set output name",   ARG_REQ, handleOutput) \
    X("h", "help",   "Show this message", ARG_OPT, handleHelp  ) \
    \
    \
    X(NULL, "disable-entry", "Skip Search entry point from code",                        ARG_NONE, handleEntryPoint) \
    X("v",  "verbose",       "Run code in verbode mode, sampling the compilation phase", ARG_NONE, handleVerbose   )

typedef struct {
    const char *shortOpt;
    const char *longOpt;
    const char *desc;

    int hasVal;
    void (*handler)(const char *val, ArgCtx *ctx);
} ArgOption;

typedef struct {
    char *outputName;
    int   searchEntryPoint;
    int   verbose;

    char *pos;
} ArgCtx;

extern ArgOption options[];
extern const int optCount;

int parseArgv(int argc, char **argv, ArgCtx *ctx);

#define X(short, long, desc, arg, handler) void handler(const char *val, ArgCtx *ctx);
    ARGS_TABLE
#undef X