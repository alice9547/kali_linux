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
    int nSockFd;                  //클라이언트 주소 길이
    struct pollfd rfds[2];   //표준입력, 소켓. 모니터링 대상
    int nTimeout = 0;       //0으로 초기화
    int nRetval;            //poll함수 리턴값 저장 변수

    char p_Buffer[BUFSIZ];
    int nBufferLen;

    struct sockaddr_in stSAddr;
    int nSAddr_size;

    if (argc != 3)          //ip 주소, 포트번호 입력 위해 3개여야만 함
    {
        printf("usage: %s <IP Address> <Port>\n", argv[0]);
        return -1;          //명령 3개 아니면 에러발생
    }
    // UDP 소켓 생성
    nSockFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    rfds[0].fd = 0;             //offset 0 위치에 0 설정
    rfds[0].events = POLLIN;    //0의 파일기술자는 입력
    rfds[0].revents = 0;        //자동생성

    rfds[1].fd = nSockFd;       //offset 0 위치에 0 설정
    rfds[1].events = POLLIN;    //0의 파일기술자는 입력
    rfds[1].revents = 0;        //자동생성

    nTimeout = 1000;
    
    bzero(&stSAddr, sizeof(stSAddr));               //서버관련 주소 정보 저장
    stSAddr.sin_family = AF_INET;
    stSAddr.sin_addr.s_addr = inet_addr(argv[1]);   //주소값
    stSAddr.sin_port = htons(atoi(argv[2]));        //포트번호
    nSAddr_size = sizeof(stSAddr);                  

    do          //exit 명령어 받기 전까지 계속 실행
    {
        nRetval = poll(rfds, 2, nTimeout);   //등록된 파일기술자 모니터링 시작
        if (nRetval < 0)        //오류는 do-while 아예 빠져나오기
            break;
        if (nRetval == 0)
            continue;

        // 키보드 입력 프로세스          
        if (rfds[0].revents & POLLIN)   //입력이 존재
        {
            bzero(p_Buffer, BUFSIZ);
            nBufferLen = read(0, p_Buffer, BUFSIZ); //키보드 입력값 기록
            nBufferLen = sendto(nSockFd, p_Buffer, nBufferLen, 0,
                (struct sockaddr*)&stSAddr, nSAddr_size);       //서버 전달
            if (nBufferLen > 0)
            {
                if (strncasecmp(p_Buffer, "EXIT", 4) == 0)
                    //대소문자 상관없이 앞부터 입력받은 값이 exit면 종료_for문만 빠져나감
                    break;

                printf("TX: %s", p_Buffer);     //exit 아니면 출력
            }
        }
        //소켓 리시버 프로세스
        if (rfds[1].revents & POLLIN)    //서버의 소켓자에서 입력 있으면 실행
        {
            bzero(p_Buffer, BUFSIZ);        //버퍼 초기화
            nBufferLen = recvfrom(nSockFd, p_Buffer, BUFSIZ, 0,
                (struct sockaddr*)&stSAddr, &nSAddr_size);   //소켓을 통해 데이터 받음
            if (nBufferLen > 0)
            {
                printf("RX: %s", p_Buffer);
            }
        }
    } while (1);

    //소켓 닫기      
    close(nSockFd);  //모니터링 대상 fd 닫기

    return 0;
}