#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "print_hostent.h"

void print_hostent(struct hostent* host)
{
	int i;

	printf("Official name of host: %s\n\n", host->h_name);
	if(host->h_aliases[0] != NULL){
		printf("==Aliases==\n");
		for( i=0; host->h_aliases[i]; i++){
			printf("%02dth: %s\n", i+1, host->h_aliases[i]);
		}

		printf("\n");
	}

	printf("Address type: %s\n\n",
		(host->h_addrtype == AF_INET) ? "AF_INET" : "AF_INET6");

	printf("IP Address: \n");
	for( i=0; host->h_addr_list[i]; i++){
			printf("%02d: %s\n", i+1, 
				inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
	}

	return;
}