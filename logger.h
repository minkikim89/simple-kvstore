#ifndef __LOGGER_H__
#define __LOGGER_H__

typedef enum loglevel {
    DEBUG = 1,
    INFO,
    WARN,
    ERROR
} LOGLEVEL;

LOGLEVEL level;

void init_logger(void);
void set_loglevel(int loglevel);
void plog(int loglevel, const char *fmt, ...);
#endif
