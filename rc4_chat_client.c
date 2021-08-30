#include <stdio.h>
#include <stdlib.h>     //atoi()
#include <sys/poll.h>   //poll()
#include <string.h>
#include <strings.h>    //zero(), strncasecmp()
#include <fcntl.h>      //rc4
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>   //rc4
#include <arpa/inet.h>
#include <time.h>       //time()
#include <unistd.h>
#include <stdbool.h>
#include <openssl/rc4.h>

#include "Network_Common.h"

int main(int argc, char* argv[])
{
    //변수
    struct pollfd rfds[2];   //서버에게 전달위함_키보드 입력값과 소켓을 위한 2개
    int nTimeout = 0;       //0으로 초기화
    int nRetval;            //리턴값 저장 변수

    unsigned char p_cKey[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };
    //암호화를 위한 비밀번호

    RC4_KEY stRc4Key;	//구조체 변수

    //변수 초기화
    for (int i = 0; i <  2; i++)
    {
        rfds[i].fd = -1;
        rfds[i].events = 0;
        rfds[i].revents = 0;
    }

    if (argc != 3)          //포트번호, 연결 대기큐 개수 입력 위해 3개여야만 함
    {
        printf("usage: %s <IP> <Port>\n", argv[0]);
        return -1;          //명령 3개 아니면 에러발생
    }

    printf("IP %s\n", argv[1]);   //IP
    printf("Port %s\n", argv[2]);  //포트
    
    rfds[0].fd = 0;            //offset 0 위치에 0 설정
    rfds[0].events = POLLIN;    //0의 파일기술자는 입력
    rfds[0].revents = 0;        //자동생성

    rfds[1].fd = connect_to_tcp_server(argv[1], atoi(argv[2]));    
    //두번째 배열은 만들어지면서 이미 서버 접속 시도하는 서버 소켓기술자 얻어옴

    if (rfds[1].fd < 0)     //소켓기술자 시작 실패하면 음수 리턴
    {
        printf("Connecting Server is failed.\n");
        return -1;          //실패하면 에러 발생
    }
    rfds[1].events = POLLIN;    //서버에서의 입력 모니터링
    rfds[1].revents = 0;
//0번,1번은 서버시작과 입력을 위한 고정된 값

    nTimeout = 1000;        //POLL은 밀리초 기준(1초)

    RC4_set_key(&stRc4Key, 16, p_cKey);	//키 생성 -> 얘로 암호화, 복호화 함
    do
    {
        nRetval = poll(rfds, 2, nTimeout);   //등록된 파일기술자 모니터링 시작

        if (nRetval > 0)        //이벤트 감지
        {
            // 키보드 입력 프로세스          
            if (rfds[0].revents & POLLIN)   //입력이 존재
            {
                char strBuffer[BUFSIZ];
                int nBufferLen = 0; //실제 입력값 길이
                int nDataLen;	//암호화된 데이터 길이
                unsigned char p_cOutputData[BUFSIZ] = { 0 };	//기록될 데이터

                bzero(strBuffer, BUFSIZ);
                nBufferLen = read(0, strBuffer, BUFSIZ); //키보드 입력값 기록 
                printf("Input : %s", strBuffer);
                if (nBufferLen > 0)
                {
                    if (strncasecmp(strBuffer, "EXIT", 4) == 0)
                    {
                        //대소문자 상관없이 앞부터 입력받은 값이 exit면 종료_for문만 빠져나감
                        break;
                    }                    
                    RC4(&stRc4Key, nBufferLen, strBuffer, p_cOutputData);	//암호화 수행
                     //(키정보, 길이, 읽어들인 데이터, 결과물 기록)
                    nDataLen = read(rfds[1].fd, p_cOutputData, BUFSIZ);

                    send(rfds[1].fd, p_cOutputData, nDataLen, 0);
                }
            }

            //소켓 리시버 프로세스
            if (rfds[1].revents & POLLIN)    //서버로부터 값이 옴
            {
                char strBuffer[BUFSIZ];
                int nBufferLen = 0;
                unsigned char p_cOutputData[BUFSIZ] = { 0 };	//기록될 데이터

                bzero(strBuffer, BUFSIZ);
                nBufferLen = read(rfds[1].fd, strBuffer, BUFSIZ);            

                RC4(&stRc4Key, nBufferLen, strBuffer, p_cOutputData);	//복호화 수행
                    //(키정보, 길이, 읽어들인 데이터, 결과물 기록)
                printf("%s", p_cOutputData);
                printf("%s", strBuffer);
            }
        }
        else if (nRetval < 0)   //폴함수 값
        {
            break;
        }
    } while (1);

    //소켓 닫기      
    close(rfds[1].fd);

    printf("Bye~\n");

    return 0;
}
