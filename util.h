#ifndef UTIL_H
#define UTIL_H

#include <sys/types.h>

typedef struct {
    char *value;
    size_t length;
} token;


bool tokenize(char *str, int str_len, token_t *tokens, int *token_len);

#endif
