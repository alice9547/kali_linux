#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h> //select 함수
#include <string.h>
#include <sys/select.h>
#include <time.h> //time
#include <unistd.h>

int main(void) 
{
    fd_set rfds;         //입력하기에 읽기 전용 파일 기술자
    struct timeval tv;   //select 함수 timeout 지정위해
    int retval;         //select 함수 결과값 저장 위해

  time_t nTime;        //현재 시간 변수

  char strBuffer[BUFSIZ] = {0, };   //키보드 값 입력 받기위함
  int nBufferLen = 0;               //키보드 값 입력 위함

  do {
      FD_ZERO(&rfds);           //rfds 모든 비트를 0으로 변환
      FD_SET(0, &rfds);        //0 bit(offset) 부분 1로 설정. 
                               //0 bit는 키보드 입력 부분 -> 키보드 입력 감시

      tv.tv_sec = 5;        //timeout 초세팅
      tv.tv_usec = 0;       

      retval = select(FD_SETSIZE, &rfds, NULL, NULL, &tv);  
                    //(최대개수, 모니터링할거(읽기 파일 기술자) 등록, write, exception, timeout)

      if (retval) { //값이 있을때
          if (FD_ISSET(0, &rfds)) {      //0bit 부분에 이벤트가 발생했는지 = 입력있음
              memset(strBuffer, 0, BUFSIZ);
              nBufferLen = read(0, strBuffer, BUFSIZ);
              // 1. 입력 확인 위해 read() 사용 
              // 2. 키보드입력을 strBuffer저장
              // 3. 저장된 길이 nBufferLen에 저장.

              if (nBufferLen > 0) {
                  if (strncasecmp(strBuffer, "exit", 4) == 0) //대소문자 가리지않는 비교 함수
                      break;

                  printf("[%02d] : %s", nBufferLen, strBuffer);
              }
          }
      } else {      //값이 없을때 (0)
          time(&nTime); //현재시간 출력
          printf("Current: %s\n", ctime(&nTime));
      }
  } while (1);

  return 0;
}
