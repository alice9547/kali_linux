#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>


int main(void) {

  int nPid = 0;
  nPid = fork();        
  //호출되는 순간부터 프로세스 2개로 나뉨
  //출력은 Child,Parent 둘다 될 것 -> 두개의 프로세스가 동일한 터미널을 공유하기에

  if (nPid == 0) {
      printf("Child. (PID: %d)\n", nPid); //한 프로세스는 이 출력문 실행
  } else{
      printf("Parent.(PID: %d)\n", nPid); //다른 프로세스는 출력문 실행.
  }

  return 0;
}
