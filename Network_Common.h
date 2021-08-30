#include <sys/types.h>
#include <unistd.h>

int start_tcp_server(uint16_t nPort, int nBacklog); 
//서버 구동(포트, 대기큐 받아서 처리)_ip는 자기거 사용,

int connect_to_tcp_server(char* strIpAddress, uint16_t nPort); 
//클라이언트용. tcp서버 접속 요청(서버IP, 포트) 후 소켓기술자 반환 
