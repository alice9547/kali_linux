#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include "Network_common.h"


int sending_time_info(int nCFd);
int sending_user_data(int nCFd);

int main(int argc, char* argv[]) {


  if (argc != 3 ) {
      printf("Usage: %s <IP> <PORT>\n",argv[0]);
      return -1;
      }

  nCFd = connect_to_tcp_server(argv[1], atoi(argv[2]));
  if (nCFd < 0) {
      return -1;
  }

  while ((nBufferLen=recv(ncfd, strBuffer, BUFSIZ, 0)) >0 ) {
        printf("Recv : %s", strBuffer);
  }

  printf("ㅃㅇ!!\n");

  close(nCFd);

  return 0;
}
//??이거 이상해 소스가 ㅅㅂ
