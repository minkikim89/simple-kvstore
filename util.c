#include <util.h>

/*
 * tokenize string
 * it transform original string
 * hello world sentence -> hello\0world\0sentence
 */
bool tokenize(char *str, int str_len, token_t *tokens, int *token_len) {
    const char* delimeter = ' ';
    int tidx = 0;
    int remain_len, checked_len = 0;
    char *tp = str;
    char *s;

    while(1) {
        s = tp + 1;
        remain_len = cmdlen - checked_len;
        tp = memchr(s, delimeter, remain_len);

        if (tp) {
            *tp = '\0';
            tokens[tidx].value = s;
            tokens[tidx].length = tp - s;
            tidx++;
            checked_len = tp - input;
        } else {
            tokens[tidx].value = s;
            tokens[tidx].length = remain_len;
            tidx++;
            break;
        }

        if(tidx >= MAX_COMMAND_LEN) {
            return false;
        }
    }

    *token_len = tidx;
    return true;
}

