#include "debug.h"

#include <stdarg.h>
#include <stdio.h>

void Debug_Log(const char *fmt, ...)
{
#ifdef HOST_BUILD
    va_list args;
    va_start(args, fmt);
    (void)vfprintf(stderr, fmt, args);
    va_end(args);
#else
    (void)fmt;
#endif
}
