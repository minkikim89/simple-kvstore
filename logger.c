#include <stdarg.h>
#include "logger.h"

void init_logger()
{
  level = WARN;
}

void set_loglevel(int loglevel)
{

}

void plog(int loglevel, const char *fmt, ...)
{
  va_list strs;
  va_start(strs, fmt);

  vprintf(fmt, strs);
}
