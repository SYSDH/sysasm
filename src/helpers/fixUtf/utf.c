#include <locale.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "../helpers.h"

void fixUtf() {
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    #endif

    setlocale(LC_ALL, ".UTF8");
}