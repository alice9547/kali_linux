#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "Network_common.h"

int main(int argc, char* argv[]) {

  int nCFd = -1;
  char strBuffer[BUFSIZ];   //데이터 받기위한 구조체
  int nBufferLen = 0;   //길이

  if (argc != 3 ) {     //3개 파라미터
      printf("Usage: %s <IP> <PORT>\n",argv[0]);
      return -1;
      }

  nCFd = connect_to_tcp_server(argv[1], atoi(argv[2])); //포트번호 문자열에서 정수형으로 바꿈_atoi함수
  if (nCFd < 0) {
      return -1;
  }

  while ((nBufferLen=recv(nCFd, strBuffer, BUFSIZ, 0)) >0 ) {   
      //recv.수신(클라이언트 소켓기술자로)을 위한 함수. nBufferLen에 수신된 데이터 리턴.
        printf("Recv : %s", strBuffer); //수신데이터 출력
  }

  printf("Bye!!\n");

  close(nCFd);

  return 0;
}
