#include <unistd.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]) {

  int nSFd; //서버 FD
  int nCFd; //클라이언트 FD
  //소켓 기술자 2개 선언 -> 접속 후 클라이언트 통신도 하기 위함.

  uint16_t  nPort; //서버 포트
  struct sockaddr_in sAddr; //서버 주소
  struct sockaddr_in cAddr; //클라이언트 주소
  socklen_t nCAddr; //클라이언트 주소 길이-> 사이즈값 얻어내기 위함

  char strBuffer[BUFSIZ];
  int nBufferLen = 0;

  if (argc != 3) {
      printf("Usage: %s <ip> <port> \n", argv[0]);        //명령, ip주소, 포트번호
      return -1;
  }

  nPort = atoi(argv[2]);      //정수값으로 포트번호 받음
  printf("IP : %s\n", argv[1]);       //파라미터 출력
  printf("Port : %d \n", nPort); //변환된 정수형 포트번호 출력

  nSFd = socket(PF_INET, SOCK_STREAM, 0);
  printf("Sever FD: %d\n", nSFd );

  bzero(&sAddr, sizeof(struct sockaddr_in));  //0으로 초기화
  sAddr.sin_family = AF_INET;
  sAddr.sin_addr.s_addr = inet_addr(argv[1]);
  //ip주소에는 문자열 ip주소를 네트워크 주소 정수형태인 big_endian로 변환된 값을 대입
  sAddr.sin_port = htons(nPort); //네트워크 순서로 바꿔야함(16bit인 2byte의 순서 바꿈)

  if (bind(nSFd, (struct sockaddr*)&sAddr,sizeof(sAddr)) < 0 ){
      printf("Fail...\n" );
      return -1;
  }

  listen(nSFd, 1); // 대기큐 1개(클라이언트 1개만 허용)
  printf("OK.\n");

  nCFd = accept(nSFd, (struct sockaddr*)&cAddr, &nCAddr); 
  //새로운 소켓기술자 생성(서버용 소켓기술자_이미 서버상태로 대기(bind로 인해), (struct sockaddr*)*cAddr에 정보 저장.)
  if (nCFd > 0) { //2보다 큰값으로 할당되어 있음_서버를 위한 소켓 + .. 등등
      printf("Client Info:\n");
      printf("\t- IP Adderss : %s \n",inet_ntoa(cAddr.sin_addr)); //네트워크 기반의 데이터를 문자열 ip주소로 변경_ntoa
      printf("\t- Port : %d \n",ntohs(cAddr.sin_port)); //네트워크 -> host로 변경

      //클라이언트는 서버와 다르게 맘대로 지정x
      nBufferLen = sprintf(strBuffer, "Hello!!\n");
      send(nCFd, strBuffer, nBufferLen, 0); //클라이언트 기술자는 연결이 안되어있어서 바로 데이터 전송 불가능.

      close(nCFd);
  }

  close(nSFd);

  return 0;
}
