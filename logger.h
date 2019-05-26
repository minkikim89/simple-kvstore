enum {
    LOGLEVEL_DEBUG = 1,
    LOGLEVEL_INFO,
    LOGLEVEL_WARNING,
    LOGLEVEL_ERROR
}

int logger_level = LOGLEVEL_WARNING;

void init_logger(void);
void set_loglevel(int loglevel);
void pr_log(int loglevel, const char *fmt, ...);
