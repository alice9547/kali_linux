#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <poll.h>
#include <time.h> //time
#include <unistd.h>

#define MAX_FDS 10

int main(void) 
{
    int nRetval = 0;       
    //poll 함수의 결과값 기록(0은 이벤트x, 양수는  발생 이벤트 개수, -1은 강제종료)
    char strBuffer[BUFSIZ];     //문자열_키보드 입력위함
    int nBufferLen = 0;         //문자열 길이_키보드 입력위함
    struct pollfd fds[MAX_FDS]; //모니터링할 파일기술자(최대 10개)

    time_t nPrevTime = 0;   //직전시간(기준)
    time_t nCurTime = 0;    //현재시간(5초 지남)
    //poll함수는 select와 다르게 timeout이 계속 바뀌지 않기에 직전시간과 현재시간 별도 지정

    for (int i = 0; i < MAX_FDS; i++) {
        fds[i].fd = -1;
        fds[i].events = 0x00;
        fds[i].revents = 0x00;
    }
    //for문으로 배열fds 초기화

        fds[0].fd = 0;          //키보드 입력 위한 표준입력 0 
        fds[0].events = POLLIN; //입력값 존재 유무
        fds[0].revents = 0;     //자동 세팅

        time(&nPrevTime);       //직전시간을 현재시간으로 설정
  do {
      nRetval = poll(fds, 1, 5000); //5초 단위로 모니터링
      if (nRetval < 0) {  //문제있으면 강제 종료
          perror("Poll exception!!\n");
          return -1;
          }
      time(&nCurTime);          //현재시간 갱신
      if (nCurTime >= nPrevTime + 5) {              //직전시간에서 5초 차이 -> 5초 지남
          printf("======>%s", ctime(&nCurTime));
          nPrevTime = nCurTime;                     //직전시간에 현재 시간 저장
      }
      if (nRetval == 0) continue;                   //do-while문 돌아감(입력x)

      if (fds[0].revents & POLLIN) {                //revents에 POLLIN있으면 True -> 키보드 입력o
          bzero(strBuffer, BUFSIZ);
          nBufferLen = 0;

          nBufferLen = read(fds[0].fd, strBuffer, BUFSIZ);  //키보드 입력값 기록  
          printf("Data:(%d): %s",nBufferLen, strBuffer);

          if (strncasecmp(strBuffer, "EXIT", 4) == 0)       
              //대소문자 상관없이 앞부터 입력받은 값이 exit면 종료
              break;
      }
  } while (1);

  printf("Exiting.\n");

  return 0;
}
