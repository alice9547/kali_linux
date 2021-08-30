#include <stdio.h>
#include <stdlib.h>     //atoi()
#include <string.h>
#include <strings.h>    //zero(), strncasecmp()
#include <sys/poll.h>   //poll()
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    //변수
    int nSocketFd;      //소켓위한 기술자
    char strBuffer[BUFSIZ];   //메세지 받기위한 버퍼
    int nBufferLen = 0;     //메세지 받기위한 버퍼
    char strBuffer2[BUFSIZ];   //메세지 받기위한 버퍼
    int nBufferLen2 = 0;     //메세지 받기위한 버퍼

    int nTimeout = 0;       //0으로 초기화
    int nRetval;            //poll함수 리턴값 저장 변수

    struct pollfd rfds[2];   //표준입력, 소켓. 모니터링 대상
    struct sockaddr_in stSAddr;     //주소정보 기록위한 구조체_서버용
    struct sockaddr_in stCAddr;     //주소정보 기록위한 구조체_클라이언트용
    int nCAddr_size;

    if (argc != 2)          
    {
        printf("usage: %s <port>\n", argv[0]);
        return -1;          //명령 2개 아니면 에러발생
    }
    //UDP 소켓 생성
    nSocketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);       //(INET기반, 타입_UDP, 세부적 프로토콜)
    
    rfds[0].fd = 0;             //offset 0 위치에 0 설정
    rfds[0].events = POLLIN;    //0의 파일기술자는 입력
    rfds[0].revents = 0;        //자동생성

    rfds[1].fd = nSocketFd;       //offset 0 위치에 0 설정
    rfds[1].events = POLLIN;    //0의 파일기술자는 입력
    rfds[1].revents = 0;        //자동생성

    //서버바인딩 위한 서버 주소 세팅
    memset(&stSAddr, 0, sizeof(stSAddr));       //서버주소 초기화
    stSAddr.sin_family = AF_INET;
    stSAddr.sin_addr.s_addr = htonl(INADDR_ANY);    //인디안 순서 바꿈
    stSAddr.sin_port = htons(atoi(argv[1]));        //명령어 다음 값을 포트번호로

    if (bind(nSocketFd, (struct sockaddr*)&stSAddr, sizeof(stSAddr)) < 0)       //바인딩 실패하면 0이 아님
    {
        printf("Binding Failed.\n");
        return -1;
    }

    //데이터 수신
    do
    {
        nRetval = poll(rfds, 2, nTimeout);   //등록된 파일기술자 모니터링 시작
        if (nRetval < 0)        //오류는 do-while 아예 빠져나오기
            break;
        if (nRetval == 0)
            continue;

        nCAddr_size = sizeof(stCAddr);

        memset(strBuffer, 0, BUFSIZ);
        // 키보드 입력 프로세스          
        if (rfds[0].revents & POLLIN)   //입력이 존재
        {
            bzero(strBuffer, BUFSIZ);
            nBufferLen = read(0, strBuffer, BUFSIZ); //키보드 입력값 기록
            printf("Input : %s", strBuffer);
           
            if (nBufferLen > 0)
            {
                if (strncasecmp(strBuffer, "EXIT", 4) == 0)
                    //대소문자 상관없이 앞부터 입력받은 값이 exit면 종료_for문만 빠져나감
                    break;

                nBufferLen2 = sprintf(strBuffer2, "[Server] %s", strBuffer);
                sendto(nSocketFd, strBuffer2, nBufferLen2, 0,
                    (struct sockaddr*)&stSAddr, nCAddr_size);       //클라이언트 전달
            }
        }
        //소켓 리시버 프로세스
        if (rfds[1].revents & POLLIN)    //클라이언트의 소켓자에서 입력 있으면 실행
        {
            bzero(strBuffer, BUFSIZ);        //버퍼 초기화
            nBufferLen = recvfrom(nSocketFd, strBuffer, BUFSIZ, 0,
                (struct sockaddr*)&stSAddr, &nCAddr_size);   //소켓을 통해 데이터 받음
            if (nBufferLen > 0)
            {
                printf("RX: %s", strBuffer);
            }
        }
    } while (1);

    close(nSocketFd);

    return 0;
}
