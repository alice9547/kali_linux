#include <stdio.h>
#include <stdlib.h>     //atoi()
#include <sys/poll.h>   //poll()
#include <string.h>
#include <strings.h>    //zero(), strncasecmp()
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <time.h>       //time()
#include <unistd.h>
#include <stdbool.h>
#include <openssl/rc4.h>
#include <fcntl.h>

#include "Network_Common.h"

#define MAX_CLIENTS 10

int main(int argc, char* argv[])
{
    //변수
    struct sockaddr_in stCAddr;         //클라이언트 주소
    socklen_t nCAddr;                   //클라이언트 주소 길이

    unsigned char p_cKey[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 }; //암호화를 위한 비밀번호
    
    RC4_KEY stRc4Key;	//구조체 변수

    struct pollfd rfds[MAX_CLIENTS + 2];   //모니터링 위한 poll용 구조체 배열 변수
                                            //MAX_CLIENTS +2 는 표준 입력, 서버 위해서, 나머지는 클라이언트

    int nTimeout = 0;       //0으로 초기화
    int nRetval;            //리턴값 저장 변수
    int nDataLen;	//암호화된 데이터 길이
    int nKeepRunning = 1;   //루프문 빠져나갈건지 판단 위한 변수

    RC4_set_key(&stRc4Key, 16, p_cKey);	//키 생성 -> 얘로 암호화, 복호화 함

    for (int i = 0; i < MAX_CLIENTS + 2; i++)
    {
        rfds[i].fd = -1;
        rfds[i].events = 0;
        rfds[i].revents = 0;
    } //for문으로 모든 클라이언트 배열 rfds 초기화

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

    //키보드
    rfds[0].fd = 0;            //offset 0 위치에 0 설정
    rfds[0].events = POLLIN;   //읽기부분 모니터링
    rfds[0].revents = 0;       //자동생성

    rfds[1].fd = start_tcp_server(atoi(argv[1]), atoi(argv[2]));    //두번째 배열은 서버 소켓기술자 얻어옴
    if (rfds[1].fd < 0)     //소켓기술자 시작 실패하면 음수 리턴
    {
        printf(" Starting Server is failed.\n");
        return -1;          //실패하면 에러 발생
    }
    rfds[1].events = POLLIN;    //서버에서의 입력 모니터링
    rfds[1].revents = 0;
//0번,1번은 서버시작과 입력을 위한 고정된 값

    nTimeout = 1000;        //POLL은 밀리초 기준(=1초)

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
                        char strBuffer[BUFSIZ];     //키보드 입력 받기위한 버퍼
                        int nBufferLen = 0;         //버퍼 크기
                        char strBuffer2[BUFSIZ];    //입력받은거 수정위한 버퍼
                        int nBufferLen2 = 0;        //수정하는 버퍼 크기
                        unsigned char p_cOutputData[BUFSIZ] = { 0 };	//기록될 데이터

                        bzero(strBuffer, BUFSIZ);   //초기화
                        bzero(strBuffer2, BUFSIZ);  //2초기화
                        nBufferLen = read(0, strBuffer, BUFSIZ); //키보드 입력값 기록 
                        printf("Input : %s", strBuffer);
                        if (nBufferLen > 0)
                        {
                            if (strncasecmp(strBuffer, "EXIT", 4) == 0)
                            {
                                //대소문자 상관없이 앞부터 입력받은 값이 exit면 종료_for문만 빠져나감
                                nKeepRunning = 0;    //1이면 계속 do-while문 수행
                                break;
                            }

                            nBufferLen2 = sprintf(strBuffer2, "[Server] %s", strBuffer);
                            RC4(&stRc4Key, nBufferLen2, strBuffer2, p_cOutputData);	//암호화, 복호화 수행
                            //(키정보, 길이, 읽어들인 데이터, 결과물 기록)


                            for (int i = 2; i < MAX_CLIENTS + 2; i++) //변수 모든 클라이언트에게 전달
                            {
                                if (rfds[i].fd < 0)
                                    continue;
                                send(rfds[i].fd, p_cOutputData, nBufferLen2, 0);
                            }
                        }
                    }
                    else if (i == 1)    //서버 소켓
                    {
                        printf("Connecting...");
                        int fd = accept(rfds[i].fd, (struct sockaddr*)&stCAddr, &nCAddr);  //클라이언트 접속 허용
                        if (fd > 0) //등록
                        {
                            for (int i = 2; i < MAX_CLIENTS + 2; i++)
                            {
                                if (rfds[i].fd < 0)         //비어있는 배열 찾기
                                {
                                    rfds[i].fd = fd;           //파일 기술자 
                                    rfds[i].events = POLLIN;    //모니터링을 위해
                                    rfds[i].revents = 0;

                                    char strBuffer[BUFSIZ];     //문자열을 만들어내기 위한 공간
                                    unsigned char p_cOutputData[BUFSIZ] = { 0 };	//기록될 데이터
                                    int nBufferLen = 0;         //문자열을 만들어내기 위한 공간
                                    int nDataLen = 0;         //암호화된 데이터 길이
                                   
                                    bzero(strBuffer, BUFSIZ);
                                    nBufferLen = sprintf(strBuffer, "[%d] Your ID is [%d]\n", fd, fd);                                   
                                    
                                    RC4(&stRc4Key, nBufferLen, strBuffer, p_cOutputData);	//암호화, 복호화 수행
                            //(키정보, 길이, 읽어들인 데이터, 결과물 기록)
                                    
                                    send(rfds[i].fd, p_cOutputData, nBufferLen, 0);
                                    break;
                                }
                            }
                            printf("Success!!");    //성공하면 양수 리턴
                        }
                        printf("\n");
                    }
                    else //접속 되어있는 클라이언트
                    {
                        char strBuffer[BUFSIZ];
                        unsigned char p_cOutputData[BUFSIZ] = { 0 };	//복호화 데이터
                        unsigned char p_cOutputData2[BUFSIZ] = { 0 };	//다시 암호화 데이터
                        int nBufferLen = 0;
                        int nDataLen = 0;   //암호화한 데이터 길이
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
                        else
                        {
                            char strBuffer2[BUFSIZ];
                            int nBufferLen2 = 0;
                            bzero(strBuffer2, BUFSIZ);

                            RC4(&stRc4Key, nBufferLen2, strBuffer2, p_cOutputData);	//복호화 수행

                            nBufferLen2 = sprintf(strBuffer2, "[%d] %s", rfds[i].fd, p_cOutputData);
                            printf("%s", strBuffer2);
                            RC4(&stRc4Key, nBufferLen2, strBuffer2, p_cOutputData2);	//암호화, 복호화 수행
                           ///////////// 크기를 어떻게 구해야할지 모르겠음
                           // nDataLen = read(rfds[i].fd, p_cOutputData2, BUFSIZ); //암호화된 데이터 길이 구하기
                           ///////////
                            for (int i = 2; i < MAX_CLIENTS + 2; i++)
                            {
                                if (rfds[i].fd < 0)
                                    continue;
                                send(rfds[i].fd, p_cOutputData2, nBufferLen2, 0);
                            }//연결되어 있는 모든 클라이언트에게 전달
                        }
                    }
                }
            }

            if (nKeepRunning == 0)  //exit에 의한 값전달
            {
                break;
            }
        }
        else if (nRetval < 0)
        {
            break;
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
