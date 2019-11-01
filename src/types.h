#ifndef __TYPES_H_
#define __TYPES_H_

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

typedef enum {false, true} bool;

typedef enum {
  COMMAND_ERR

} ERROR_CODE;

#define PAGE_SIZE (4*1024) // 4K

#endif
