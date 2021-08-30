#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
	struct in_addr stAddr;
	unsigned char* p = (unsigned char*)&stAddr;

	if (argc != 2)
	{
		return -1;
	}
	printf("IP Address is %s\n", argv[1]);

	if (inet_aton(argv[1], &stAddr) != 1)
	{
		perror("Failed to convert the address.");
		return 0;
	}

	printf("Inet Addr is 0x%X\n", stAddr);
	printf("\t[0]=0x%02X\n", p[0]);
	printf("\t[1]=0x%02X\n", p[1]);
	printf("\t[2]=0x%02X\n", p[2]);
	printf("\t[3]=0x%02X\n", p[3]);
	
	return 0;
}
