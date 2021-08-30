#include <stdio.h>
#include <stdlib.h>     //atoi()
#include <sys/poll.h>   //poll()
#include <string.h>
#include <strings.h>    //zero(), strncasecmp()
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>       //time()
#include <unistd.h>

#include "Network_Common.h"

#define MAX_CLIENTS 10

int main(int argc, char* argv[])
{
    //변수
    struct sockaddr_in stCAddr;         //클라이언트 주소
    socklen_t nCAddr;                   //클라이언트 주소 길이

    struct pollfd rfds[MAX_CLIENTS + 2];   //모니터링 위한 poll용 구조체 배열 변수
                                            //MAX_CLIENTS +2 는 표준 입력, 서버 위해서, 나머지는 클라이언트

    int nTimeout = 0;       //0으로 초기화
    int nRetval;            //리턴값 저장 변수

    char strBuffer[BUFSIZ]; //문자열 초기화_키보드 입력위함
    int nBufferLen = 0;     //문자열 길이 초기화_키보드 입력위함

    //생성자 변수
    bzero(strBuffer, BUFSIZ);
    for (int i = 0; i < MAX_CLIENTS + 2; i++)
    {
        rfds[i].fd = -1;
        rfds[i].events = 0;
        rfds[i].revents = 0;
    }
    //for문으로 배열 rfds 초기화

    if (argc != 3)          //포트번호, 연결 대기큐 개수 입력 위해 3개여야만 함
    {
        printf("usage: %s <port> <queue>\n", argv[0]);
        return -1;          //명령 3개 아니면 에러발생
    }

    printf("Port %s\n", argv[1]);   //포트
    printf("Queue %s\n", argv[2]);  //대기큐
    if (atoi(argv[2]) > MAX_CLIENTS)   //대기큐(문자를 정수형으로 변환)는 MAX_CLIENTS 보다 크면 안됨
    {
        return -1;          //최대값보다 크기에 에러 발생
    }

    rfds[0].fd = 0;            //offset 0 위치에 0 설정
    rfds[0].events = POLLIN;    //0의 파일기술자는 입력
    rfds[0].revents = 0;        //자동생성

    rfds[1].fd = start_tcp_server(atoi(argv[1]), atoi(argv[2]));    //두번째 배열은 서버 소켓기술자 얻어옴
    if (rfds[1].fd < 0)     //소켓기술자 시작 실패하면 음수 리턴
    {
        printf(" Starting Serner is failed.\n");
        return -1;          //실패하면 에러 발생
    }
    rfds[1].events = POLLIN;    //서버에서의 입력 모니터링
    rfds[1].revents = 0;

//0번,1번은 서버시작과 입력을 위한 고정된 값

    nTimeout = 1000;        //POLL은 밀리초 기준(1초)

    do
    {
        nRetval = poll(rfds, MAX_CLIENTS + 2, nTimeout);   //등록된 파일기술자 모니터링 시작

        if (nRetval > 0)        //이벤트 감지
        {
            for (int i = 0; i < MAX_CLIENTS + 2; i++)   //FOR문
            {
                if (rfds[i].fd < 0)     //음수면 잘못 들어온거기에 계속 실행
                    continue;

                if (rfds[i].revents & POLLIN)   //입력이 존재
                {
                    if (i == 0)         //표준 입력(키보드 입력)
                    {
                        bzero(strBuffer, BUFSIZ);
                        nBufferLen = read(0, strBuffer, BUFSIZ); //키보드 입력값 기록 
                        printf("Input : %s",strBuffer);
                        if (nBufferLen > 0)
                        {
                            if (strncasecmp(strBuffer, "EXIT", 4) == 0) 
                                //대소문자 상관없이 앞부터 입력받은 값이 exit면 종료_for문만 빠져나감
                                break;
                        }                                            
                        nBufferLen = 0; //바깥에서는 안보이기에 0으로 재초기화_안하면 다시 위 if문 돌수있음
                    }
                    else if (i == 1)    //서버 소켓
                    {
                        printf("Connecting...");
                        int fd = accept(rfds[i].fd, (struct sockaddr *)&stCAddr, &nCAddr);  //클라이언트 접속 허용
                        if (fd > 0)
                        {
                            for (int i = 2; i < MAX_CLIENTS + 2; i++)
                            {
                                if (rfds[i].fd < 0)         //비어있는 배열 찾기
                                {
                                    rfds[i].fd = fd;           //파일 기술자 
                                    rfds[i].events = POLLIN;    //모니터링을 위해
                                    rfds[i].revents = 0;
                                    break;
                                }
                            }
                            printf("Success!!");    //성공하면 양수 리턴
                        }
                        printf("\n");
                    }
                    else //클라이언트
                    {
                        bzero(strBuffer, BUFSIZ);
                        nBufferLen = read(rfds[i].fd, strBuffer, BUFSIZ);
                        if (nBufferLen <= 0)        //접속 해제
                        {
                            printf("A Client(%d) is disconnected. \n", rfds[i].fd);
                            close(rfds[i].fd);      //닫기
                            rfds[i].fd = -1;        //fd에서 삭제
                            rfds[i].events = 0;     //모니터링 대상에서도 삭제
                            rfds[i].revents = 0;
                        }
                        else {
                            printf("A Client(%d) MSG: %s\n", rfds[i].fd, strBuffer);    
                            if (strncasecmp(strBuffer, "exit", 4) == 0) {
                                printf("Client is closed\n");
                                close(rfds[i].fd);      //닫기
                                rfds[i].fd = -1;        //fd에서 삭제
                                rfds[i].events = 0;     //모니터링 대상에서도 삭제
                                rfds[i].revents = 0;
                            }                                
                            bzero(strBuffer, BUFSIZ);       //다시 초기화
                            nBufferLen = 0;                 //길이도 초기화
                        }
                        
                    }
                }
            }

            if (nBufferLen > 0)
            {
                if (strncasecmp(strBuffer, "exit", 4) == 0)
                    break;
            }
            //do-while문 빠져나감
        }
        else if (nRetval == 0)          //Timeout 발생 -> 이벤트 처리 x
        {
            time_t nTime;           //시간 저장할 변수

            time(&nTime);           //현재시간 추출
            nBufferLen = sprintf(strBuffer, "%s", ctime(&nTime)); //추출한 시간 정보 문자열로 변환
            for (int i = 2; i < MAX_CLIENTS + 2; i++)       //연결된 모든 클라이언트
            {
                if (rfds[i].fd < 0)     //실패했을때는 넘기기
                    continue;

                send(rfds[i].fd, strBuffer, nBufferLen, 0);     
                //모든 클라이언트에게 문자열로 변환한 시간 정보 전송
            }
        }
        else //poll함수 리턴값이 음수면 문제o
        {
            break; //빠져나감
        }
    } while (1);

    //모든 파일기술자 닫기
        for (int i = 0; i < MAX_CLIENTS + 2; i++)   //서버, 입력, 모든 클라이언트
        {
            if (rfds[i].fd > 0)     
                close(rfds[i].fd);  //모니터링 대상 fd 닫기
        }
        printf("Bye~\n");

        return 0;
   }
