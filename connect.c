#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>


int main(int argc, char* argv[]) {
  int fd = -1;
  struct sockaddr_in sock_client;

  if (argc !=3) { //인자 3개로 봄_명령어 자기자신, 파라미터 2개
      printf("Usage: %s <ip> <port> \n", argv[0]);  //명령어 <ip 주소> <포트>
      return -1;
  }

  memset(&sock_client, 0, sizeof(sock_client)); //주소를 지정해야하기에 0으로 초기화

  //구조체 설정.
  sock_client.sin_family = AF_INET; //ipv4 버전 사용
  
  sock_client.sin_addr.s_addr = (in_addr_t)inet_addr(argv[1]); 
  //argv[1]는 ip주소. 4바이트 정수형 -> 네트워크 주소체계. big_endian으로 변경(강제 형변환)
  
  sock_client.sin_port = htons(atoi(argv[2]));
  //포트번호를 atoi함수 사용해서 정수형으로 변환 후 htons로 네트워크 체계로 변환

  fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //소켓기술자

  if (connect(fd,(struct sockaddr*)&sock_client,sizeof(sock_client)) == 0) {
      //소켓기술자를 통해 주소 대입(명령어를 받아 이미 세팅)
      printf("Success.\n");
      } else {
        printf("Fail.\n" );
      }
  printf("Press Enter to finish\n" );
  getchar();

  close(fd);

  return 0;
}
