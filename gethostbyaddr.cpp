#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "print_hostent.h"

int main(int argc, char* argv[])
{
	struct hostent* host;
	uint32_t addr = 0;

	if(argc != 2){
		printf("usage: %s domain name\n", argv[0]);
		return -1;
	}

	addr = inet_addr(argv[1]);
	host = gethostbyaddr(&addr, 4, AF_INET);
	if(!host){
		printf("gethostbyaddr function error\n");
		return -1;
	}

	print_hostent(host);

	printf("Done.\n");
	
	return 0;
}
