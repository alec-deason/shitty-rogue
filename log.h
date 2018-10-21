#ifndef LOG_H
#define LOG_H

#include <stdbool.h>

extern bool logging_active;

int logger(const char * format, ... );

#endif
