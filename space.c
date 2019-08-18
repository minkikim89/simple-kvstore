#include <stdio.h>
#include <stdlib.h>
#include "space.h"

char* slot_alloc(int size)
{
  char *ptr =  malloc(size);
  if (ptr == NULL) {
    //TODO
  }
  return ptr;
}

void slot_free(void *ptr)
{
  free(ptr);
}
