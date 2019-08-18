#ifndef __STORAGE_H__
#define __STORAGE_H__
#include "util.h"
int set_operation(token_t key_token, token_t value_token);
int get_operation(token_t key_token, token_t *value_token);
int del_operation(token_t key_token);
#endif
