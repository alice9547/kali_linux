#include <stdio.h> //디폴트로 사용
#include <unistd.h> // 디폴트 사용
#include <sys/socket.h> //소켓 관련 필수
#include <sys/types.h> //소켓 관련 필수
#include <fcntl.h> //파일 관련 필수
#include <string.h> //문자열 처리

#define MAX_FD 4    // MAX_FD 4로 메크로 정의

int main(void) {

  int fd[MAX_FD] = {-1, };  // 배열로 선언
  int i = 0;  //for문 위해

  printf("Process ID : %d\n", getpid() );

      for (i = 0; i < MAX_FD; i++){
          fd[i] = socket(PF_INET, SOCK_STREAM, 0);  
          //파일 기술자(소켓 기술자) 값 대입
    }
  printf("Press Enter to close.\n" );
  getchar();        //키보드 입력

      for (i = 0; i < MAX_FD; i++) {
        close(fd[i]);       //환원. 닫기
      }
  printf("Press Enter to finish.\n" );
  getchar();

  return 0;
}
