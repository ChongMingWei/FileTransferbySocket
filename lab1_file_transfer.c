#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h> 				// to get file size
#include <time.h>					//to get current time
#include <string.h>					//to determine the protocol and role(server or client)
void error(const char *msg)
{
    perror(msg);
    exit(1);
}
void printlog(float cur,float acc);	//function to log time while file transfering
void Tcp_Server(char *para[]);
void Tcp_Client(char *para[]);
void Udp_Server(char *para[]);
void Udp_Client(char *para[]);
int main(int argc, char *argv[]){
	if(!strcmp("tcp",argv[1])){							//strcmp:Check if it is "tcp".If two strings are the same,return 0.
		if(!strcmp("send",argv[2]) && argc == 6)		//Identify the role(server or client) and check parameters
			Tcp_Server(argv);
		else if(!strcmp("recv",argv[2]) && argc == 5)	
			Tcp_Client(argv);
		else
			printf("Check the parameters again!!\n");
	}
	else if(!strcmp("udp",argv[1])){					//strcmp:Check if it is "udp".If two strings are the same,return 0.
		if(!strcmp("send",argv[2]) && argc == 6)		//Identify the role(server or client) and check parameters
			Udp_Server(argv);
		else if(!strcmp("recv",argv[2]) && argc == 5)	
			Udp_Client(argv);
		else
			printf("Check the parameters again!!\n");
	}
	else{
		printf("Check the parameters again!!\n");
	}
	return 0;
}

void Tcp_Server(char *para[]){
	printf("welcome to tcp_server\n");
	int i = 0;
	for(i = 0;i < 6;++i)
		printf("%s ",para[i]);
	printf("\n");
}
void Tcp_Client(char *para[]){
	printf("welcome to tcp_client\n");
	int i = 0;
	for(i = 0;i < 5;++i)
		printf("%s ",para[i]);
	printf("\n");
}
void Udp_Server(char *para[]){
	printf("welcome to udp_server\n");
	int i = 0;
	for(i = 0;i < 6;++i)
		printf("%s ",para[i]);
	printf("\n");
}
void Udp_Client(char *para[]){
	printf("welcome to udp_client\n");
	int i = 0;
	for(i = 0;i < 5;++i)
		printf("%s ",para[i]);
	printf("\n");
}