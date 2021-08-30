#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(void)
{
	struct in_addr addr;
	struct in_addr addr2;
	char* ip_addr;
	char* ip_addr2;

	addr.s_addr = htonl(0x01020304);
	addr2.s_addr = htonl(0x12345678);

	printf("Network Address is 0x%X\n", addr);
	printf("Network Address2 is 0x%X\n", addr2);

	ip_addr = inet_ntoa(addr);
	ip_addr2 = inet_ntoa(addr2);

	printf("IP Address is %s\n", ip_addr);
	printf("IP Address2 is %s\n", ip_addr2);
	
	return 0;
}
