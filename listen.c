#include <unistd.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]) {

  int nFd;      //서버를 위한 기술자
  uint16_t nPort;       //포트번호
  struct sockaddr_in sAddr;     //서버를 위한 주소

    if (argc !=3) {
        printf("Usage: %s <ip> <port> \n", argv[0]);        //명령, ip주소, 포트번호
        return -1;
    }

    nPort = atoi(argv[2]);      //정수값으로 포트번호 받음
    printf("IP : %s\n", argv[1] );       //파라미터 출력
    printf("Port : %d \n", nPort); //변환된 정수형 포트번호 출력

    nFd = socket(PF_INET, SOCK_STREAM, 0); //소켓 만듦(TCP의 의미인 SOCK으로 생성)
    printf("Sever FD: %d.\n", nFd);

    bzero(&sAddr, sizeof(struct sockaddr_in));  //0으로 초기화
    sAddr.sin_family = AF_INET;     
    sAddr.sin_addr.s_addr = inet_addr(argv[1]); 
    //ip주소에는 문자열 ip주소를 네트워크 주소 정수형태인 big_endian로 변환된 값을 대입
    sAddr.sin_port = htons(nPort); //네트워크 순서로 바꿔야함(16bit인 2byte의 순서 바꿈)


    if (bind(nFd, (struct sockaddr*)&sAddr,sizeof(sAddr)) < 0 ){
        // bind 함수로 소켓, 포트 번호 연결함
        printf("Fail...\n");      //바인딩 실패하면 리턴값 -1.
        return -1;
    }

    listen(nFd, 10); // 만든 소켓에 대해 대기큐 10개 생성
    printf("OK.\n");

    printf("Enter to finish.\n");
    getchar();

    close(nFd);

  return 0;
}
