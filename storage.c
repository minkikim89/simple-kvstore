#include "server.h"

int set_operation(char *key_str, int key_len, char *value_str, int value_len)
{
  int size = key_len + value_len;
  slot_alloc(size);

  insert_to_index();
  return 0;
}
