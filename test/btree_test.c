#include <stdio.h>
#include <string.h>
#include "sys/socket.h"
#include "netinet/in.h"

#define BUF_SIZE 1024

int main (int argc, char **argv)
{
  char *host = "127.0.0.1";
  char wbuf[BUF_SIZE];
  char rbuf[BUF_SIZE];
  int port = 12345;
  int cfd, n, i;
  struct sockaddr_in server_addr;

  cfd = socket(PF_INET, SOCK_STREAM, 0);
  if (cfd < 0) {
    printf("Failed to call socket()\n");
    return -1;
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(host);
  server_addr.sin_port = htons(port);

  if (connect(cfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    printf("Failed to call connect()\n");
    return -1;
  }

  // insert test
  for (i = 0; i < 100; i++) {
    int s = sprintf(wbuf, "set test-%d %d\r\n", i, i);
    printf("%s", wbuf);
    write(cfd, wbuf, s);
  }
  //read(cfd, rbuf, BUF_SIZE);
  close(cfd);

  //delete test
  /*
  for (i = 99; i >= 0 ; i--) {
    int s = sprintf(wbuf, "delete test-%d\r\n", i, i);
    printf("%s", wbuf);
    write(cfd, wbuf, s);
    //read(cfd, rbuf, BUF_SIZE);
  }
  */

  return 0;
}
