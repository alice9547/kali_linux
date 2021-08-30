#include <stdio.h>
#include <stdlib.h>     //atoi()
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    //변수
    int nSocketFd;
    char pBuffer[BUFSIZ];
    int nBufferLen = 0;

    struct sockaddr_in stSAddr;
    struct sockaddr_in stCAddr;
    int nCAddr_size;

    if (argc != 2)          
    {
        printf("usage: %s <port>\n", argv[0]);
        return -1;          //명령 2개 아니면 에러발생
    }
    //UDP 소켓 생성
    nSocketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    memset(&stSAddr, 0, sizeof(stSAddr));
    stSAddr.sin_family = AF_INET;
    stSAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    stSAddr.sin_port = htons(atoi(argv[1]));

    if (bind(nSocketFd, (struct sockaddr*)&stSAddr, sizeof(stSAddr)) < 0)
    {
        printf("Binding Failed.\n");
        return -1;
    }

    do
    {
        nCAddr_size = sizeof(stCAddr);
        memset(pBuffer, 0, BUFSIZ);
        nBufferLen = recvfrom(nSocketFd, pBuffer, BUFSIZ, 0, (struct sockaddr*)&stCAddr, &nCAddr_size);

        if (nBufferLen > 0)
        {
            printf("Client Information:\n");
            printf("Addr: %s\n", inet_ntoa(stCAddr.sin_addr));
            printf("Port: %d\n", ntohs(stCAddr.sin_port));
            printf("RX: %s\n", pBuffer);
        }
        else if (nBufferLen <= 0)
            break;

        nBufferLen = sendto(nSocketFd, pBuffer, nBufferLen, 0, 
                            (struct sockaddr*)&stCAddr, nCAddr_size);
    } while (1);

    close(nSocketFd);

    return 0;
}
