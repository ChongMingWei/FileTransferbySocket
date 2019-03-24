/*************************************************************************
    > File Name: echoser_udp.c
    > Author: Simba
    > Mail: dameng34@163.com
    > Created Time: Sun 03 Mar 2013 06:13:55 PM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>

#define ERR_EXIT(m) \
    do { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while (0)


int main(int argc, char *argv[])
{
    int sock,portno;
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        ERR_EXIT("socket error");

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
	portno = atoi(argv[1]);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(portno);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("bind error");


    char recvbuf[1024];
	char buffer[16];
    struct sockaddr_in peeraddr;
    socklen_t peerlen;
    int n;
	peerlen = sizeof(peeraddr);
	memset(recvbuf, 0, sizeof(recvbuf));
    n = recvfrom(sock, recvbuf,sizeof(recvbuf), 0, (struct sockaddr *)&peeraddr, &peerlen);
	fputs(recvbuf, stdout);
	sendto(sock, "start", sizeof("start"), 0, (struct sockaddr *)&peeraddr, peerlen);

	
	/*send file*/
	FILE *fp;
	if((fp = fopen(argv[2],"rb")) == NULL){
		perror("fopen");
		exit(1);
	}
	
	while(1){
		if(feof(fp)){
			strcpy(buffer, "end");
			sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&peeraddr, peerlen);
			break;
		}
		n = fread(buffer, sizeof(char), sizeof(buffer), fp);
		sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&peeraddr, peerlen);
		bzero(buffer,sizeof(buffer));
	}
	strcpy(buffer, "end");
	sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&peeraddr, peerlen);

	
	fclose(fp);
    close(sock);

    return 0;
}
