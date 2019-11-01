#include <string.h>
#include "storage.h"
#include "types.h"
#include "server.h"
#include "space.h"
#include "btree.h"

int set_operation(token_t key_token, token_t value_token)
{
  int size = sizeof(uint64_t) + key_token.length + sizeof(uint64_t) + value_token.length;
  void *origin = slot_alloc(size);
  char *ptr = origin;
  *(uint64_t*)ptr = key_token.length;

  ptr = ptr + sizeof(uint64_t);
  memcpy(ptr, key_token.value, key_token.length);

  ptr = ptr + key_token.length;
  *(uint64_t*)ptr = value_token.length;

  ptr = ptr + sizeof(uint64_t);
  memcpy(ptr, value_token.value, value_token.length);

  insert_index(key_token.value, key_token.length, origin);
  return 0;
}

int get_operation(token_t key_token, token_t *value_token)
{
  char *ptr;
  int ret = search_index(key_token.value, key_token.length, (void**)&ptr);
  if (ret < 0) {
    return -1;
  }
  int value_length = *(ptr + sizeof(uint64_t) + key_token.length);
  char *value = ptr + sizeof(uint64_t) + key_token.length + sizeof(uint64_t) ;

  value_token->value = value;
  value_token->length = value_length;
  return 0;
}

int del_operation(token_t key_token)
{
  int ret;
  char *ptr;
  ret = search_index(key_token.value, key_token.length, (void**)&ptr);
  if (ret < 0) {
    return 0;
  }
  delete_index(key_token.value, key_token.length);
  slot_free(ptr);
  return 0;
}
