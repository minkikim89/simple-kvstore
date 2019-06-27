#ifndef WORKER_H
#define WORKER_H

#include "server.h"

struct worker_global {
  int worker_count;
};

typedef struct _worker {
  client *client_head;
} worker;

void assign_client_to_worker(client *c);
bool create_worker(pthread_t *thr_ids, int thread_num);

#endif
