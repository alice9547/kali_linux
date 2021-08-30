#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>

#include "Network_common.h"     //공통파일 include

int sending_time_info(int nCFd);    //함수 정의

int main(int argc, char* argv[]) {  //파라미터로 받아야하기에 argument로 받음


  int nSFd;     //서버용 소켓기술자 
  int nCFd;     //클라이언트용 소켓기술자

  struct sockaddr_in cAddr;     //클라이언트 위해 주소정보 저장할 구조체 선언_서버는 함수로 만들어놔서 필요x
  socklen_t nCAddr;

  if (argc != 2) {  //명령 개수가 부족할때
      printf("usage: %s <port>\n", argv[0] );
      return -1;
      }

  printf("Port %s\n", argv[1]);
  nSFd = start_tcp_server(atoi(argv[1]), 1);        
  //서버 구동_문자열을 정수형으로 변환(atoi함수_32bit여도 내부적으로 16bit로 자름)
  //대기큐 개수는 1개.(클라이언트 접속 하나만 허용)
  
  if (nSFd > 0 ) {
      printf("TCP is started.\n");
    }else{
      printf("Fail...\n" );
      return -1;        //강제종료
    }

  nCFd = accept(nSFd, (struct sockaddr*)&cAddr, &nCAddr);   //클라이언트가 접속 요청하면 수락함. 수락전까지 대기탐
  if (nCFd > 0) {
      printf("Client Info: \n");
      printf("\t- IP Address : %s\n", inet_ntoa(cAddr.sin_addr));
      printf("\t- Port : %s\n", ntohs(cAddr.sin_port));

      sending_time_info(nCFd);      //시간 데이터를 클라이언트쪽으로 보냄(밑에 선언)
      close(nCFd);
  }
  else {
      printf("Fail.\n");

  }

    close(nSFd);    //소켓기술자 닫기
    return 0;
  }

//-------------------------------------------------------//

    int sending_time_info(int nCFd) {

      char strBuffer[BUFSIZ];       //문자열 형태
      int nBufferLen = 0;           //길이

      for (int i = 0; i < 10; i++) {
           
            struct timeval stTimeval;
            struct tm stTm;

            gettimeofday(&stTimeval, 0);    //나노 초 단위까지 추출 가능
            localtime_r(&stTimeval.tv_sec, &stTm);  //유닉스 단위. 초단위를 추출해서 년월일 추출
            nBufferLen = sprintf(strBuffer,"%04d/%02d/%02d %02d:%02d:%02d:.%03d\n",
            stTm.tm_year + 1900,
            stTm.tm_mon + 1,
            stTm.tm_mday + 1,
            stTm.tm_hour,
            stTm.tm_min,
            stTm.tm_sec,
            stTimeval.tv_usec / 1000);

            send(nCFd, strBuffer, nBufferLen, 0);   //네트워크로 데이터 전송 함수

            sleep(1);       //1초 쉼
            }
    }
