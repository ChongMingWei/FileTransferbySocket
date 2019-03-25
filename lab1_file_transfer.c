#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/stat.h> 				// to get file size
#include <time.h>					//to get current time
#include <string.h>					

void error(const char *msg)
{
    perror(msg);
    exit(1);
}
void Tcp_Server(char *para[]);
void Tcp_Client(char *para[]);
void Udp_Server(char *para[]);
void Udp_Client(char *para[]);
void printlog(float cur,float acc);	//to print file sending progress
int main(int argc, char *argv[]){
	if(!strcmp("tcp",argv[1])){			//strcmp:Check if it is "tcp".If two strings are the same,return 0.
		if(!strcmp("send",argv[2]))		//Identify the role(server or client) and check parameters
			Tcp_Server(argv);
		else if(!strcmp("recv",argv[2]))	
			Tcp_Client(argv);
		else
			printf("Check the parameters again!!\n");
	}
	else if(!strcmp("udp",argv[1])){	//strcmp:Check if it is "udp".If two strings are the same,return 0.
		if(!strcmp("send",argv[2]))		//Identify the role(server or client) and check parameters
			Udp_Server(argv);
		else if(!strcmp("recv",argv[2]))	
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
	 int sockfd, newsockfd, portno;				//sockfd for server's socket and newsockfd for client's connection
     socklen_t clilen;
     char buffer[16];
     struct sockaddr_in serv_addr, cli_addr;
	 /*Socket build part*/
     sockfd = socket(AF_INET, SOCK_STREAM, 0);	//TCP setting(select SOCK_STREAM)
     if (sockfd < 0)
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(para[4]);
	 /*socket setting for server*/
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
	 /*accept the connection of client*/
     newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);
     if (newsockfd < 0)
          error("ERROR on accept");
	  
	 /*File sending part*/
	 int numbytes;					//the #byte which is sent to client each time
	 FILE *fp;
	 /*Get file size, to compute the file sending progress*/
	 struct stat st;
	 stat(para[5], &st);
	 int file_size = st.st_size;	//st_size is a varible of struct stat which stores the size of a file
	 /*Open binary file and read(use parameter "rb")*/
	 if((fp = fopen(para[5],"rb")) == NULL){
		error("fopen");
		exit(1);
	}
	 /*Start sending file*/
	 float accratio = 0;			//the upper bound of each transfering(accumulate)
	 float currentratio = 0;		//the lower bound of each transfering(current)
     while(1){
		 /*if get the end of file, print the last log*/
		 if(feof(fp)){
			 time_t rawtime;
			 struct tm * timeinfo;
			 rawtime = time(NULL);
			 timeinfo = localtime(&rawtime);
			 printf("100%\t%d/%d/%d\t%d:%d:%d\n",timeinfo->tm_year+1900,timeinfo->tm_mon,timeinfo->tm_mday
					,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
			 break;
		 }
		 numbytes = fread(buffer, sizeof(char), sizeof(buffer), fp);
		 numbytes = write(newsockfd, buffer, numbytes);
		 accratio += (float)numbytes / (float)file_size;
		 printlog(currentratio, accratio);
		 currentratio = accratio;
	 }
	 fclose(fp);
     close(newsockfd);
     close(sockfd);
}

void Tcp_Client(char *para[]){
	int sockfd, portno, n;					 //sockfd for client's socket
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[16];
	/*Socket build part*/
    portno = atoi(para[4]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);//TCP setting(select SOCK_STREAM)
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(para[3]);		//Get server IP here
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
	/*socket setting for client and connecting to server*/
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
	
    /*File receiving part*/
	FILE *fp;
	int numbytes;							//the #byte which is received from server each time
	/*Open binary file and write in(use parameter "wb")*/
	if((fp = fopen(para[5],"wb")) == NULL){
		error("fopen");
		exit(1);
	}
	/*Start receiving file*/
	while(1){
		numbytes = read(sockfd, buffer, sizeof(buffer));
		printf("read %d bytes, ", numbytes);
		if(numbytes == 0){
			break;
		}
		numbytes = fwrite(buffer, sizeof(char), numbytes, fp);
		printf("fwrite %d bytes\n", numbytes);
	}
	fclose(fp);
    close(sockfd);
}

void Udp_Server(char *para[]){
    int sock,portno;		//sockfd for server's socket
	struct sockaddr_in servaddr;
	char recvbuf[1024];		
	char buffer[16];		
	/*Socket build part*/
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)//TCP setting(select SOCK_DGRAM)
        error("socket error");
    memset(&servaddr, 0, sizeof(servaddr));
	portno = atoi(para[4]);
	/*socket setting for server*/
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(portno);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        error("bind error");
	/*get the connection of client*/
    struct sockaddr_in peeraddr;
    socklen_t peerlen;
    int n;		//the #byte which is sent to or received from client each time
	/*accept the connection of client and start connection*/
	peerlen = sizeof(peeraddr);
	memset(recvbuf, 0, sizeof(recvbuf));
    n = recvfrom(sock, recvbuf,sizeof(recvbuf), 0, (struct sockaddr *)&peeraddr, &peerlen);
	sendto(sock, "start", sizeof("start"), 0, (struct sockaddr *)&peeraddr, peerlen);

	/*File sending part*/
	FILE *fp;
	/*Open binary file and read(use parameter "rb")*/
	if((fp = fopen(para[5],"rb")) == NULL){
		error("fopen");
		exit(1);
	}
	/*Get file size, to compute the file sending progress*/
	struct stat st;
	stat(para[5], &st);
	int file_size = st.st_size;	//st_size is a varible of struct stat which stores the size of a file
	/*Start sending file*/
	float accratio = 0;			//the upper bound of each transfering(accumulate)
	float currentratio = 0;		//the lower bound of each transfering(current)

	while(1){
		/*if get the end of file, print the last log and send end signal*/
		if(feof(fp)){
			time_t rawtime;
			struct tm * timeinfo;
			rawtime = time(NULL);
			timeinfo = localtime(&rawtime);
			printf("100%\t%d/%d/%d\t%d:%d:%d\n",timeinfo->tm_year+1900,timeinfo->tm_mon,timeinfo->tm_mday
					,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
			/*end signal*/
			sendto(sock, "end", sizeof("end"), 0, (struct sockaddr *)&peeraddr, peerlen);
			break;
		}
		n = fread(buffer, sizeof(char), sizeof(buffer), fp);
		sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&peeraddr, peerlen);
		accratio += (float)n / (float)file_size;
		printlog(currentratio, accratio);
		currentratio = accratio;
		bzero(buffer,sizeof(buffer));
	}
	sendto(sock, "end", sizeof("end"), 0, (struct sockaddr *)&peeraddr, peerlen);
	fclose(fp);
    close(sock);
}

void Udp_Client(char *para[]){
	int sock,portno;		//sock for client's socket
	struct sockaddr_in servaddr;
	char buffer[16];
	/*Socket build part*/
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        error("socket");
    memset(&servaddr, 0, sizeof(servaddr));
	portno = atoi(para[4]);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(portno);
    servaddr.sin_addr.s_addr = inet_addr(para[3]);

	/*start signal*/
	sendto(sock, "hello!", strlen("hello!"), 0,(struct sockaddr *)&servaddr, sizeof(servaddr));

    /*File receiving part*/
	FILE *fp;
	int n;		//the #byte which is received from server each time
	/*Open binary file and write in(use parameter "wb")*/
	if((fp = fopen(para[5],"wb")) == NULL){
		perror("fopen");
		exit(1);
	}
	/*Start receiving file*/
	recvfrom(sock, buffer, sizeof(buffer), 0,NULL, NULL);
	while(1){
		if(!strcmp(buffer, "end")){
			break;
		}
		n = fwrite(buffer, sizeof(char), n, fp);
		printf("fwrite %d bytes\n", n);
		n = recvfrom(sock, buffer, sizeof(buffer), 0,NULL, NULL);
		printf("read %d bytes, ", n);
		
	}
	puts("completed");
	fclose(fp);
    close(sock);
}

void printlog(float cur,float acc){
	int lastlog = cur*100;		//the upper bound of each transfering(accumulate)
	int curlog = acc*100;		//the lower bound of each transfering(current)
	int i;
	/*setting for printing current time*/
	time_t rawtime;
	struct tm * timeinfo;
	rawtime = time(NULL);
	timeinfo = localtime(&rawtime);
	
	for(i=lastlog;i<curlog;++i){
		if(i!=0&&i%5 == 0)
			printf("%d%\t%d/%d/%d\t%d:%d:%d\n",i,timeinfo->tm_year+1900,timeinfo->tm_mon,timeinfo->tm_mday
					,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec); 
	}
}