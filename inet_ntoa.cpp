#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(void)
{
	struct in_addr addr;
	char* ip_addr;

	addr.s_addr = htonl(0x01020304);
	printf("Network Address is 0x%X\n", addr);

	ip_addr = inet_ntoa(addr);

	printf("IP Address is %s\n", ip_addr);
	
	return 0;
}
