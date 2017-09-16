#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

#define MAXLINE 4096

void str_cli(int in, int out, int connfd)
{
	char sendline[MAXLINE+1];
	char recvline[MAXLINE];
	int sendlen;
	int recvlen;
	
	while (1) {
		memset(sendline, 0x00, sizeof(sendline));
		memset(recvline, 0x00, sizeof(recvline));
		
		sendlen = read(in, sendline, MAXLINE);
		if (sendlen <= 0) {
			printf("read in err: %s (errno: %d) \n", strerror(errno), errno);
			break;
		}
		
		recvlen = write(connfd, sendline, sendlen);
		if (recvlen <= 0) {
			printf("write connfd err: %s (errno: %d) \n", strerror(errno), errno);
			break;
		}
		
		recvlen = read(connfd, recvline, MAXLINE);
		if (recvlen <= 0) {
			printf("read connfd err: %s (errno: %d) \n", strerror(errno), errno);
			break;
		}
		
		sendlen = write(out, recvline, recvlen);
		if (sendlen <= 0) {
			printf("write out err: %s (errno: %d) \n", strerror(errno), errno);
			break;
		}
		
	} 
}

int main(int argc, char * argv[]) 
{
	int port = 15000;
	char *ip = "127.0.0.1";
	
	if (argc == 3) {
		strcpy(ip, argv[1]);
		port = atoi(argv[2]);
	}
	
	int connfd = 0;
	struct sockaddr_in servaddr;
	
	// create socket
	if ((connfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("create socket err: %s (errno: %d) \n", strerror(errno), errno);
		exit(errno);
	}
	
	// set servaddr
	memset(&servaddr, 0x00, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &servaddr.sin_addr.s_addr);
	
	// connect
	if (connect(connfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
		printf("connect err: %s (errno: %d) \n", strerror(errno), errno);
		exit(errno);
	}
	
	// send message
	str_cli(STDIN_FILENO, STDOUT_FILENO, connfd);
	
	// close
	close(connfd);
	
	return 0;
}
