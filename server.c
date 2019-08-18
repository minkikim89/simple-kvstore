#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sysexits.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include "server.h"
#include "storage.h"
#include "logger.h"
#include "types.h"
#include "util.h"
#include "errno.h"

#define EPOLL_SIZE      20

#define MAX_BUFFER_SIZE 1024

#define COMMAND_TOKEN 0
#define KEY_TOKEN 1
#define VALUE_TOKEN 2

struct server server;

static void signal_shutdown_handler(int sig)
{
  if (sig == SIGTERM || sig == SIGINT)
  {
    plog(WARN, "shutdown signal\n");
    server.shutdown = 1;
  }
}

static int setup_signal_handler()
{
  struct sigaction act;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  act.sa_handler = signal_shutdown_handler;
  sigaction(SIGTERM, &act, NULL);
  sigaction(SIGINT, &act, NULL);
  return 0;
}

client* create_client(int fd)
{
  client *c = malloc(sizeof(client));
  c->fd = fd;
  c->querybuf = malloc(MAX_BUFFER_SIZE);
  if (c->querybuf == NULL) {
    return NULL;
  }
  c->outbuf = malloc(MAX_BUFFER_SIZE);
  if (c->outbuf == NULL) {
    return NULL;
  }

  return c;
}


int reply(client *c, char *str, int slen)
{
  //TODO
  char buf_in[258];
  memcpy(buf_in, str, slen);
  memcpy(buf_in+slen, "\r\n", 2);

  write(c->fd, buf_in, slen+2);
  return 0;
}

static int
query_parse(client *c, char *query, int query_len)
{
  int ret;
  token_t tokens[3];
  int token_count;
  tokenize(query, query_len, tokens, &token_count);

  if (strcmp(tokens[COMMAND_TOKEN].value, "set") == 0) {
    ret = set_operation(tokens[KEY_TOKEN], tokens[VALUE_TOKEN]);
    if (ret == 0) {
      reply(c, "OK", 2);
    } else {
      //TODO
    }
  } else if (strcmp(tokens[COMMAND_TOKEN].value, "get") == 0) {
    token_t data_token;
    ret = get_operation(tokens[KEY_TOKEN], &data_token);
    if (ret == 0) {
      reply(c, data_token.value, data_token.length);
    } else {
      //TODO
    }
  } else if (strcmp(tokens[COMMAND_TOKEN].value, "delete") == 0) {
    ret = del_operation(tokens[KEY_TOKEN]);
    if (ret == 0) {
      reply(c, "OK", 2);
    } else {
      //TODO
    }
  }
  return 0;
}

/********************* worker ***********************/
#define MAX_WORKER_NUM  1024

typedef struct _worker {
  client    *c;
  pthread_t  tid;
} worker;

struct worker_global {
  worker  worker_list[MAX_WORKER_NUM];
  int     worker_count;
};

static struct worker_global worker_gl;

//FIXME first.. it run as single thread..

/*
void assign_client_to_worker(client *c)
{
  
}
*/

static void* worker_thread(void *arg)
{
  //TODO multi client support per thread
  worker *w = (worker*)arg;
  client *c = w->c;
  int sfd = c->fd;
  char *buff = c->querybuf;

  //TODO dynaimic buffer
  while (1) {
    int rlen = read(sfd, buff, MAX_BUFFER_SIZE);
    if (rlen <= 0) {
      printf("rlen=%d",rlen);
      close(sfd);
      plog(WARN, "Close fd\n");
      break;
    } else {
      char *tp = memchr(buff, '\n', rlen);
      if (!tp) {
        printf("not permitted command\n");
        continue;
      }
      if (*(tp - 1) == '\r') tp--;
      *tp = '\0';
      int cmdlen = tp - buff;
      query_parse(c, buff, cmdlen);
    }
  }
  return 0;
}

int worker_start(client *c)
{
  worker *worker = &worker_gl.worker_list[worker_gl.worker_count++];
  worker->c = c;

  pthread_t tid;

  int ret = pthread_create(&tid, NULL, worker_thread, worker);
  if (ret != 0) {
    plog(WARN, "Failed to create worker thread\n");
    return -1;
  }
  worker->tid = tid;

  return 0;
}

int worker_init()
{
  memset(&worker_gl, 0, sizeof(worker_gl));

  return 0;
}


int main(int argc, char **argv)
{
  int opt;
  //bool daemonize;
  //int thread_count;
  int port = 12345;
  //    int tokenlen;
  //    token tokens[MAX_COMMAND_LEN];

  while(-1 != (opt = getopt(argc, argv,
          "p"  /* port */
          "d"  /* daemonize */
          "t:" /* thread number */
          "h"  /* help */
          ))) {
    switch(opt) {
      case 'p':
        port = atoi(optarg);
      case 'd':
        //daemonize = true;
        break;
      case 't':
        //thread_count = atoi(optarg);
        break;
      case 'h':
        break;
    }
  }

  setup_signal_handler();
  logger_init();
  worker_init();

  struct epoll_event ev, *events;

  int epoll_fd;
  int client_fd;
  struct sockaddr_in server_addr, client_addr;
  int i, n;

  events = (struct epoll_event *)malloc(sizeof(*events) * EPOLL_SIZE);

  if ((epoll_fd = epoll_create(100)) < 0) {
    perror("epoll_create error");
    return 1;
  }

  server.fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server.fd == -1) {
    perror("socket error");
    close(server.fd);
    return 1;
  }
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  int flags = 1;
  setsockopt(server.fd, SOL_SOCKET, SO_REUSEADDR, (void *)&flags, sizeof(flags));

  if (bind(server.fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    perror("bind error");
    close(server.fd);
    return 1;
  }

  listen(server.fd, 5);

  memset(&ev, 0, sizeof(struct epoll_event));
  ev.events = EPOLLIN;
  ev.data.fd = server.fd;
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server.fd, &ev);

  client *c;
  while (!server.shutdown)
  {
    n = epoll_wait(epoll_fd, events, EPOLL_SIZE, -1);
    if (n == -1)
    {
      if (errno == EINTR) continue;
      else perror("epoll_wait : ");;
    }

    for (i = 0; i < n; i++)
    {
      if (events[i].data.fd == server.fd) {
        socklen_t clilen = sizeof(client_addr);
        client_fd = accept(server.fd, (struct sockaddr*)&client_addr, &clilen);
        c = create_client(client_fd);
        worker_start(c);
      } else {
      }
    }
  }
  free(events);
  close(server.fd);

  /*
     if(!tokenize(input_buffer, 12, tokens, &tokenlen)) {
     printf("Command error");
     return EX_USAGE;
     }

     worker_ids = worker_create(thr_ids, DEFAULT_THREAD_NUM);
     */
  return 0;
}
