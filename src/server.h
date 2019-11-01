#include <signal.h>

#ifndef SERVER_H
#define SERVER_H

#define MAX_COMMAND_LEN 4

#define COMMAND_TOKEN 0
#define KEY_TOKEN 1
#define VALUE_TOKEN 2

#define DEFAULT_THREAD_NUM 6

#define DEFAULT_BUFFER_SIZE 1024

typedef struct _client {
  int fd;
  char *querybuf; /* buffer per client */
  char *outbuf; /* read buffer */
  struct _client *next;
} client;

struct server {
  int fd;
  volatile sig_atomic_t shutdown;
};

#endif
