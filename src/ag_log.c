#include <stdio.h>
#include <stdarg.h>

#include "ag_log.h"

#include <windows.h>
#include <stdio.h>

const char* ag_uptime(void) {
    static char buffer[16];

    static LARGE_INTEGER frequency;
    static LARGE_INTEGER start;

    if (frequency.QuadPart == 0)
    {
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&start);
    }

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    long long ms =
        (now.QuadPart - start.QuadPart) * 1000 / frequency.QuadPart;

    long long hours = ms / 3600000;
    ms %= 3600000;

    long long minutes = ms / 60000;
    ms %= 60000;

    long long seconds = ms / 1000;
    ms %= 1000;

    // MMMM — миллисекунды, дополняемые нулями
    snprintf(
        buffer,
        sizeof(buffer),
        "%02lld:%02lld:%02lld:%04lld",
        hours,
        minutes,
        seconds,
        ms
    );

    return buffer;
}

void agLog(const char* format, ...) {
    va_list args;

    printf("[AvxGraphs] [%s] ", ag_uptime());

    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("\n");
}