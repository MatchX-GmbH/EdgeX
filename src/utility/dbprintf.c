#include "dbprintf.h"

void dbprintf(const char* fmt, ...)
{
  if(!pconfig.print_debug) return;

  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}

void dbprintk(const char* fmt, ...)
{
  if(!pconfig.print_debug) return;

  va_list args;
  va_start(args, fmt);
  //vprintk(fmt, args);
  va_end(args);
}

// console control commands

void dbprint_enable(char** args, size_t count){
  pconfig_load();
  pconfig.print_debug = true;
  pconfig_save();

  printf("Debug prints enabled!\n");
}

void dbprint_disable(char** args, size_t count){
  pconfig_load();
  pconfig.print_debug = false;
  pconfig_save();

  printf("Debug prints disabled!\n");
}
