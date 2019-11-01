#ifndef __LOGGER_H__
#define __LOGGER_H__

typedef enum loglevel {
    DEBUG = 1,
    INFO,
    WARN,
    ERROR
} LOGLEVEL;

LOGLEVEL level;

void logger_init(void);
void set_loglevel(unsigned int loglevel);
void plog(unsigned int loglevel, const char *fmt, ...);
#endif
