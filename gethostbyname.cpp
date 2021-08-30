#include <stdio.h>
#include <unistd.h>
#include <netdb.h>

#include "print_hostent.h"

int main(int argc, char* argv[])
{
	struct hostent* host;

	if(argc != 2){
		printf("usage: %s domain name\n", argv[0]);
		return -1;
	}

	host = gethostbyname(argv[1]);
	if(!host){
		printf("gethostbyname function error\n");
		return -1;
	}

	print_hostent(host);

	printf("Done.\n");
	
	return 0;
}
