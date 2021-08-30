#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "Network_common.h"

int start_tcp_server(uint16_t nPort, int nBacklog) //commmon 헤더에서 정의
{
    struct sockaddr_in stSAddr; //운영체제가 알아서 세팅하게끔 변수로 정의.
    int nSFd = -1; //number sever file describe (파일 기술자)

    nSFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (nSFd < 0) { //성공했으면 양수.
        printf("Fail.\n");
        return -1;
    }

    bzero(&stSAddr, sizeof(stSAddr)); //바인딩 위해 0으로 초기화
    stSAddr.sin_family = AF_INET;
    stSAddr.sin_addr.s_addr = htonl(INADDR_ANY); //로컬컴퓨터 IP주소. INADDR_ANY는 운영체제가 알아서 세팅(0.0.0.0)
    stSAddr.sin_port = htons(nPort); //위에 받은 파라미터 처리

    if (bind(nSFd, (struct sockaddr*)&stSAddr, sizeof(stSAddr)) < 0) { //소켓과 IP,포트 연결위해 bind함수 사용
        printf("binding Fail...\n"); //정상 실행은 0, 오류는 음수 반환이기에
        close(nSFd);
        return -1;
    }

    if (listen(nSFd, nBacklog) != 0) { //연결을 위한 입력한 파리미터만큼 대기큐 생성
        printf("listenning Fail \n");
        close(nSFd);    // 소켓 닫기
        return -1;
    }

    return nSFd;
}

    int connect_to_tcp_server(char* strIpAddress, uint16_t nPort) { //접속할 서버의 IP주소, 2바이트 포트번호
        int nCFd = -1; //소켓 기술자
        struct sockaddr_in stCAddr; //접속할 대상의 정보를 구조체에 저장위해.

        memset(&stCAddr, 0, sizeof(stCAddr));   //0으로 초기화.

        nCFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);   //소켓 생성
        
        if (nCFd < 0) { //잘못 생성되면 -1로 반환
            printf("Creating socket was Failed");
            return -1;
        }

        stCAddr.sin_family = AF_INET;
        stCAddr.sin_addr.s_addr = (in_addr_t)inet_addr(strIpAddress); //문자열이기에 네트워크 주소로 변환(int_addr함수)
        stCAddr.sin_port = htons(nPort);    //호스트 컴퓨터의 포트번호기에 네트워크 형태로 순서 변환

        if (connect(nCFd, (struct sockaddr*)&stCAddr, sizeof(stCAddr)) == 0) { 
            //접속시도(connect함수로 (struct sockaddr*)&stCAddr로 접속) 성공하면 0 반환
            printf("Success.\n");
        }
        else {
            printf("Fail.\n");
            close(nCFd);    //열린 소켓 닫기
            nCFd = -1;
        }

        return nCFd;        // 완벽하게 다 되면 소켓기술자 반환
}
