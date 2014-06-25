#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>

#include "utils.h"
#include "printpacket.c"

typedef struct sockaddr_un arg_t;

void *bridge_task(void *args) {
  struct sockaddr_un addr[2];
  int fd[2];
  socklen_t socklen = sizeof(struct sockaddr_un);
  addr[0] = ((arg_t *)args)[0];
  addr[1] = ((arg_t *)args)[1];
  
  ERRHAND(-1, fd[0] = socket(AF_UNIX, SOCK_RAW, 0));
  ERRHAND(-1, fd[1] = socket(AF_UNIX, SOCK_RAW, 0));
  ERRHAND(-1, bind(fd[0], (struct sockaddr *)&addr[0], socklen));
  ERRHAND(-1, bind(fd[1], (struct sockaddr *)&addr[1], socklen));

  while (1) {
    int bytesRead;
    unsigned char buf[65536];
    ERRHAND(-1, bytesRead = recv(fd[0], buf, 65536, 0));
    processEN10MB(buf);
    ERRHAND(-1, send(fd[1], buf, bytesRead, 0));
  }  

  return NULL;
}
    
#define DEFAULT_ADDR_A "/tmp/dt_sock_a"
#define DEFAULT_ADDR_B "/tmp/dt_sock_b"

int main() {

  struct sockaddr_un addr[2];
  addr[0].sun_family = AF_UNIX; strcpy(addr[0].sun_path, DEFAULT_ADDR_A);
  addr[1].sun_family = AF_UNIX; strcpy(addr[1].sun_path, DEFAULT_ADDR_B);

  bridge_task(addr);

  return 0;
}
