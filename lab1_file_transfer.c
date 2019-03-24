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
#include <string.h>					//to determine the protocol and role(server or client)
#define ERR_EXIT(m) \
    do { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while (0)
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
void printlog(float cur,float acc);	//to print file sending progress
int main(int argc, char *argv[]){
	if(!strcmp("tcp",argv[1])){							//strcmp:Check if it is "tcp".If two strings are the same,return 0.
		if(!strcmp("send",argv[2]))		//Identify the role(server or client) and check parameters
			Tcp_Server(argv);
		else if(!strcmp("recv",argv[2]))	
			Tcp_Client(argv);
		else
			printf("Check the parameters again!!\n");
	}
	else if(!strcmp("udp",argv[1])){					//strcmp:Check if it is "udp".If two strings are the same,return 0.
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
	 int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[16];
     struct sockaddr_in serv_addr, cli_addr;

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(para[4]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd,
                 (struct sockaddr *) &cli_addr,
                 &clilen);
     if (newsockfd < 0)
          error("ERROR on accept");
	  
	 /*file sending*/
	 int numbytes;
	 //char file_name[] = strdup(para[5]);

	 printf("the file name is %s\n",para[5]);
	 FILE *fp;
	 /*get file size*/
	 struct stat st;
	 stat(para[5], &st);
	 int file_size = st.st_size;
	 
	 fp = fopen(para[5],"rb");

	 
	 float accratio = 0;
	 float currentratio = 0;
	 
	 
     while(1){
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
		 accratio += (float)numbytes / (float)file_size;//compute how many content has been sent
		 printlog(currentratio, accratio);
		 currentratio = accratio;
		 
	 }
	 fclose(fp);
     close(newsockfd);
     close(sockfd);
}

void Tcp_Client(char *para[]){
	int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[16];

    portno = atoi(para[4]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(para[3]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
	
    /*file receiving*/
	FILE *fp;
	int numbytes;
	int get_name = 1;
	if((fp = fopen(para[5],"wb")) == NULL){
		perror("fopen");
		exit(1);
	}
	
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
    int sock,portno;
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        ERR_EXIT("socket error");
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
	
	portno = atoi(para[4]);
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
	sendto(sock, "start", sizeof("start"), 0, (struct sockaddr *)&peeraddr, peerlen);

	/*send file*/
	FILE *fp;
	if((fp = fopen(para[5],"rb")) == NULL){
		perror("fopen");
		exit(1);
	}
	/*get file size*/
	struct stat st;
	stat(para[5], &st);
	int file_size = st.st_size;
	float accratio = 0;
	float currentratio = 0;

	while(1){
		if(feof(fp)){
			time_t rawtime;
			struct tm * timeinfo;
			rawtime = time(NULL);
			timeinfo = localtime(&rawtime);
			printf("100%\t%d/%d/%d\t%d:%d:%d\n",timeinfo->tm_year+1900,timeinfo->tm_mon,timeinfo->tm_mday
					,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
					
			/*end signal*/
			strcpy(buffer, "end");
			sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&peeraddr, peerlen);
			
			break;
		}
		n = fread(buffer, sizeof(char), sizeof(buffer), fp);
		sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&peeraddr, peerlen);
		/*log file senting*/
		accratio += (float)n / (float)file_size;//compute how many content has been sent
		printlog(currentratio, accratio);
		currentratio = accratio;
		
		bzero(buffer,sizeof(buffer));
	}
	strcpy(buffer, "end");
	sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&peeraddr, peerlen);

	
	fclose(fp);
    close(sock);

}

void Udp_Client(char *para[]){
	int sock,portno;
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        ERR_EXIT("socket");

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
	portno = atoi(para[4]);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(portno);
    servaddr.sin_addr.s_addr = inet_addr(para[3]);


    int n;
    char buffer[16];
	/*start signal*/
	sendto(sock, "hello!", strlen("hello!"), 0,(struct sockaddr *)&servaddr, sizeof(servaddr));
	
    /*receive file*/
	FILE *fp;
	if((fp = fopen(para[5],"wb")) == NULL){
		perror("fopen");
		exit(1);
	}
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
	int lastlog = cur*100;
	int curlog = acc*100;
	int i;
	
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