#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>    //������ ��� ����
#include <string.h>
#include <unistd.h>

void* thread_Function(void* arg);   //������ ������ ���� �Լ� ������Ÿ�� ����

int main(void) {
  int value = 10;
  pthread_t pthread;    //������ ����ü ����

  pthread_create(&pthread, NULL, thread_Function, (void*)&value);   //(void*) ĳ����
  //������ ����(�ּ�, �����忡 ���� ��ü�� �Ӽ�, ������� ���۽�ų �Լ�_�������� ���డ��_�б�,�Լ����� ������ ��)

  for (int i = 0; i < 10; i++) {
      printf("Value : %d\n",value );
      sleep(1);     //1�ʾ� ��
  }

  pthread_join(pthread, NULL);  
  //for�� ���� �� ������ ���Ḧ ��ٸ��� ���� �޾ƾ������� NULL�̶� �� �ȹ���

  return 0;
}

void* thread_Function(void* arg){   //�Լ� ������Ÿ�� ����
  int* value = (int*)arg;

for (int i = 0; i < 10; i++ ){
    (*value)++;
    sleep(1);
    }

return NULL;
}
