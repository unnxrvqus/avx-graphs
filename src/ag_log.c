#include <stdio.h>
#include <stdarg.h>

#include "ag_log.h"

void ag_log(const char* format, ...) {
    va_list args;

    printf("[AvxGraphs] ");

    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("\n");
}