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
#include "server.h"
#include "logger.h"
#include "types.h"
#include "worker.h"
#include "util.h"

#define EPOLL_SIZE      20

//pthread_t worker_ids;

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


static int
query_parse(char *query, int query_len)
{
  token_t tokens[3];
  int token_count;
  tokenize(query, query_len, tokens, &token_count);

  if (strcmp(tokens[COMMAND_TOKEN].value, "set") == 0) {
    set_operation(tokens[KEY_TOKEN].value, tokens[KEY_TOKEN].length,
                  tokens[VALUE_TOKEN].value, tokens[VALUE_TOKEN].length);
  } else if (strcmp(tokens[COMMAND_TOKEN].value, "get") == 0) {

  }

}

int main(int argc, char **argv)
{
  int opt;
  bool daemonize;
  int thread_count;
  bool shutdown_request = false;
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
        daemonize = true;
        break;
      case 't':
        thread_count = atoi(optarg);
        break;
      case 'h':
        break;
    }
  }

  setup_signal_handler();
  init_logger();

  struct epoll_event ev, *events;

  int epoll_fd;
  int server_fd, client_fd;
  struct sockaddr_in server_addr, client_addr;
  int i, n, readn;

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

  ev.events = EPOLLIN;
  ev.data.fd = server.fd;
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server.fd, &ev);

  while(!server.shutdown)
  {
    n = epoll_wait(epoll_fd, events, EPOLL_SIZE, -1);
    if (n == -1 )
    {
      perror("epoll wait error");
    }

    for (i = 0; i < n; i++)
    {
      if (events[i].data.fd == server.fd) {
        int clilen = sizeof(client_addr);
        client_fd = accept(server.fd, (struct sockaddr*)&client_addr, &clilen);
        client *c = create_client(client_fd);
        assign_client_to_worker(c);
        ev.events = EPOLLIN;
        ev.data.fd = client_fd;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);
      } else {
        char buf_in[256];
        memset(buf_in, 0x00, 256);
        readn = read(events[i].data.fd, buf_in, 255);
        if (readn <= 0)
        {
          epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, events);
          close(events[i].data.fd);
          plog(WARN, "Close fd\n");
        } else {
          query_parse(buf_in, readn);
        }
      }
    }
  }
  close(server.fd);

  /*
     if(!tokenize(input_buffer, 12, tokens, &tokenlen)) {
     printf("Command error");
     return EX_USAGE;
     }

     worker_ids = worker_create(thr_ids, DEFAULT_THREAD_NUM);
     */
}
