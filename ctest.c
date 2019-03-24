/*************************************************************************
    > File Name: echocli_udp.c
    > Author: Simba
    > Mail: dameng34@163.com
    > Created Time: Sun 03 Mar 2013 06:13:55 PM CST
 ************************************************************************/

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define ERR_EXIT(m) \
        do \
        { \
                perror(m); \
                exit(EXIT_FAILURE); \
        } while(0)

int main(int argc, char *argv[])
{
    int sock,portno;
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        ERR_EXIT("socket");

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
	portno = atoi(argv[2]);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(portno);
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);


    int n;
    char buffer[16];
	sendto(sock, "hello!", strlen("hello!"), 0,(struct sockaddr *)&servaddr, sizeof(servaddr));
	
    /*receive file*/
	FILE *fp;
	if((fp = fopen(argv[3],"wb")) == NULL){
		perror("fopen");
		exit(1);
	}
	recvfrom(sock, buffer, sizeof(buffer), 0,NULL, NULL);
	while(1){
		if(!strcmp(buffer, "end"))
			break;
		n = fwrite(buffer, sizeof(char), n, fp);
		printf("fwrite %d bytes\n", n);
		n = recvfrom(sock, buffer, sizeof(buffer), 0,NULL, NULL);
		printf("read %d bytes, ", n);
		
	}
	puts("completed");
	fclose(fp);
    close(sock);

    return 0;
}