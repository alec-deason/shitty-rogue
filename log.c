#include <stdio.h>
#include <stdarg.h>

#include "log.h"

bool logging_active = false;

int logger(const char * format, ... ) {
    int count;
    va_list args;
    if (logging_active) {
        va_start(args, format);
        count = vfprintf(stderr, format, args);
        fflush(stderr);
        return count;
    }
}
