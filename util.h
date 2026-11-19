#ifndef UTIL_H
#define UTIL_H
#include <stdio.h>
#include <stdarg.h>

static inline int vprintlnf(const char* fmt, va_list va) {
    char buf[1024]; /* 1024 characters maximum!!! Should be enough for printing a line. */
    vsnprintf(buf, sizeof(buf), fmt, va);
    return printf("%.*s\n", (int)sizeof(buf), buf);
}

static inline int printlnf(const char* fmt, ...) {
    int n;
    va_list va;
    va_start(va, fmt);
        n = vprintlnf(fmt, va);
    va_end(va);
    return n;
}

#define WSA_PRINTERRORLN(func) printlnf(#func " failed! code: %d", WSAGetLastError())

#endif
