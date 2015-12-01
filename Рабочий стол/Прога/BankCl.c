#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

const int MaxRequest = 512;
const int MaxResponse = 512;
const int Port = 52000;

int main(int argc, char **argv)
{
	char *request = (char *)malloc(MaxRequest * sizeof(char));
	bzero(request, MaxRequest);
	printf("Please, enter your request\n");
	fgets(request,MaxRequest + 1, stdin);
	printf("%s\n", request);
	int sockfd, n;
	char *recieved = (char *)malloc(MaxResponse * sizeof(char));
	struct sockaddr_in servaddr;

	if (argc != 2)
	{
		exit(1);
	}

	bzero(recieved, MaxResponse);
	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror(NULL);
		exit(1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(Port);

	if (inet_aton(argv[1], &servaddr.sin_addr) == 0)
	{
		close(sockfd);
		exit(1);
	}

	if(connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
	{
		exit(1);
	}

	if ((n = write(sockfd, request, strlen(request) + 1)) < 0)
	{
		perror("Can't write\n");
		close(sockfd);
		exit(1);
	}

	if ((n = read(sockfd, recieved, MaxResponse - 1)) < 0)
	{
		perror("Can't read\n");
		close(sockfd);
		exit(1);
	}
	printf("%s", recieved);
	free(request);
	free(recieved);
	return 0;
}
