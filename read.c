#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void) {

  time_t nTime;
  char strBuffer[BUFSIZ];
  int nBufferLen = 0;

  for (int i = 0; ;i++) {      //무한루프

      memset(strBuffer, 0, BUFSIZ);
      nBufferLen = read(0, strBuffer, BUFSIZ); 
      // 1. 입력 확인 위해 read() 사용 
      // 2. 키보드입력을 strBuffer저장
      // 3. 저장된 길이 nBufferLen에 저장.

      if (nBufferLen > 0) {
          if (strncasecmp(strBuffer, "exit", 4) == 0) //대소문자 가리지않는 비교 함수
              break;

          prinf("[%02d] : %s", nBufferLen, strBuffer);
      } 
        printf("Recv: %s", strBuffer );     //Enter키가 입력되면 수신함
      }

      sleep(5);     //5초 대기
      time(&nTime); //현재시간 출력

      printf("Current: %s\n",ctime(&nTime));
  }

  return 0;
}
