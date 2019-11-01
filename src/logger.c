#include <stdarg.h>
#include <stdio.h>
#include "logger.h"

void logger_init()
{
  level = DEBUG;
}

void set_loglevel(unsigned int loglevel)
{
  level = loglevel;
}

void plog(unsigned int loglevel, const char *fmt, ...)
{
  if (loglevel < level)
    return;
  va_list strs;
  va_start(strs, fmt);

  vprintf(fmt, strs);
}
