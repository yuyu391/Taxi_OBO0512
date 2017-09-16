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
void str_echo(int connfd) 
{
	char line[MAXLINE+1];
	int n = 0;

	while (1) {
		memset(line, 0x00, sizeof(line));
		n = read(connfd, line, MAXLINE);
		if (n <= 0) {
			printf("client is quit!\n");
			break;
		}
		
		write(connfd, line, n);
	}


}

void sig_chld(int signa) 
{
	int 			status;
	pid_t 			tmppid;
	
	while ((tmppid = waitpid(-1, &status, WNOHANG)) > 0) {
		if (WIFEXITED(status)) {
			printf("chlid pid=%d is exit by %d\n", tmppid, WEXITSTATUS(status));
		}
		
		if (WIFSIGNALED(status)) {
			printf("chlid pid=%d is exit by %d\n", tmppid, WTERMSIG(status));
		}
	}
}


int main(int argc, char * argv[])
{
	int port = 15000;
	
	// variables declaration
	int 				sockfd, connfd;
	struct sockaddr_in 	servaddr, cliaddr;
	pid_t				pid;
	socklen_t			len;
	
	// create socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("create socket err: %s (errno: %d)\n", strerror(errno), errno);
		exit(1);
	}
	// set servaddr
	memset(&servaddr, 0x00, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	// bind
	if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
		printf("bind err: %s (errno: %d)\n", strerror(errno), errno);
		close(sockfd);
		exit(1);
	}
	
	// listen
	if (listen(sockfd, 128) == -1) {
		printf("listen err: %s (errno: %d)\n", strerror(errno), errno);
		close(sockfd);
		exit(1);
	}
	
	// signal
	signal(SIGCHLD, sig_chld);
	
	// accept
	while (1) {
		len = sizeof(cliaddr);
		if ( (connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len)) == -1 ) {
			continue;
		} 
		
		if ((pid = fork()) == 0) {
			close(sockfd);
			str_echo(connfd);
			close(connfd);
			exit(0);
		}
		
		close(connfd);
	}
	
	// close
	close(sockfd);
	
	return 0;
}
