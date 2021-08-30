#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>    //쓰레드 사용 위해
#include <string.h>
#include <unistd.h>

void* thread_Function(void* arg);   //쓰레드 동작을 위한 함수 프로토타입 정의

int main(void) {
  int value = 10;
  pthread_t pthread;    //쓰레드 구조체 변수

  pthread_create(&pthread, NULL, thread_Function, (void*)&value);   //(void*) 캐스팅
  //쓰레드 생성(주소, 쓰레드에 대한 구체적 속성, 쓰레드로 동작시킬 함수_다중으로 실행가능_분기,함수에게 전달할 값)

  for (int i = 0; i < 10; i++) {
      printf("Value : %d\n",value );
      sleep(1);     //1초씩 쉼
  }

  pthread_join(pthread, NULL);  
  //for문 종료 후 쓰레드 종료를 기다리고 값을 받아야하지만 NULL이라 값 안받음

  return 0;
}

void* thread_Function(void* arg){   //함수 프로토타입 정의
  int* value = (int*)arg;

for (int i = 0; i < 10; i++ ){
    (*value)++;
    sleep(1);
    }

return NULL;
}
