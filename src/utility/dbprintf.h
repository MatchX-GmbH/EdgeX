#ifndef DPRINTF_H
#define DPRINTF_H

#include <stdarg.h>
#include "printf.h"
#include "pconfig.h"

// Utility functions to enable log analysis of execution when needed

void dbprintf(const char* fmt, ...);
void dbprintk(const char* fmt, ...);

#endif
