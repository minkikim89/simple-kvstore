#include "server.h"
#include "types.h"
//#include "worker.h"
//#include "util.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sysexits.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define EPOLL_SIZE      20

//pthread_t worker_ids;
typedef struct _client {
    char *querybuf; /* buffer per client */
} client;

int main(int argc, char **argv) {
    int opt;
    char input_buffer[DEFAULT_BUFFER_SIZE];
//    int tokenlen;
//    token tokens[MAX_COMMAND_LEN];

    while(-1 != (opt = getopt(argc, argv,
         "d"  /* daemonize */
         "t:" /* thread number */
         "h"  /* help */
         ))) {
        switch(opt) {
        case 'd':
            printf("d");
            break;
        case 't':
            break;
        case 'h':
            break;
        }
    }
    //gets_s(input_buffer, sizeof(input_buffer)); 야탑가서 대체할 것 책에서 찾아서 쓰자.. 디버그 용

    struct epoll_event ev,*events;

    events = (struct epoll_event *)malloc(sizeof(*events) * EPOLL_SIZE);

    if ((efd = epoll_create(100)) < 0)
    {
        perror("epoll_create error");
        return 1;
    }

  clilen = sizeof(clientaddr);

  int sfd;
  int cfd;

  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1)
  {
      perror("socket error :");
      close(sfd);
      return 1;
  }
  addr.sin_family = AF_INET;
  addr.sin_port = htons(atoi(argv[1]));
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(sfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
  {
      close(sfd);
      return 1;
  }
  listen(sfd, 5);

  ev.events = EPOLLIN;
  ev.data.fd = sfd;
  epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &ev);

  while(1)
  {
      n = epoll_wait(efd, events, EPOLL_SIZE, -1);
      if (n == -1 )
      {
          perror("epoll wait error");
      }

      for (i = 0; i < n; i++)
      {
          if (events[i].data.fd == sfd)
          {
              printf("Accept\n");
              cfd = accept(sfd, (SA *)&clientaddr, &clilen);
              ev.events = EPOLLIN;
              ev.data.fd = cfd;
              epoll_ctl(efd, EPOLL_CTL_ADD, cfd, &ev);
          }
          else
          {
              memset(buf_in, 0x00, 256);
              readn = read(events[i].data.fd, buf_in, 255);
              if (readn <= 0)
              {
                  epoll_ctl(efd, EPOLL_CTL_DEL, events[i].data.fd, events);
                  close(events[i].data.fd);
                  printf("Close fd\n", cfd);
              }
              else
              {
                  printf("read data %s\n", buf_in);
                  write(events[i].data.fd, buf_in, readn);
              }
          }
      }
  }
  strncpy(input_buffer, "set k1 dddd", sizeof(input_buffer));

    if(!tokenize(input_buffer, 12, tokens, &tokenlen)) {
        printf("Command error");
        return EX_USAGE;
    }

    worker_ids = worker_create(thr_ids, DEFAULT_THREAD_NUM);
}
