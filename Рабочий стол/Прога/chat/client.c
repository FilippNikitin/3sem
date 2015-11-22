#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    int sockfd;
    char sendline[1000];
    struct sockaddr_in servaddr, cliaddr;

    if (argc != 2)
    {
        printf("Usage: ./a.out <IP address>\n");
        exit(1);
    }

    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror(NULL);
        exit(1);
    }

    bzero(&cliaddr, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(0);
    cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr*)&cliaddr, sizeof(cliaddr)) < 0)
    {
        perror(NULL);
        close(sockfd);
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(53000);
    if (inet_aton(argv[1], &servaddr.sin_addr) == 0)
    {
        printf("Invalid IP address\n");
        close(sockfd);
        exit(1);
    }

    int id = fork();

    if (id > 0)
    {
        printf("Enter your nickname:");
        scanf("%s", &sendline);

        if (sendto(sockfd, sendline, strlen(sendline) + 1, 0,
            (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
        {
            perror(NULL);
            close(sockfd);
            exit(1);
        }


        while (1)
        {
            fgets(sendline, 1000, stdin);
            if (sendto(sockfd, sendline, strlen(sendline) + 1, 0,
                (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
                {
                    perror(NULL);
                    close(sockfd);
                    exit(1);
                }
        }
    }

    if (id == 0)
    {
        char* recv = (char*)malloc (1000);
        int n;
        while(1)
        {
            for (n = 0; n < 1000; n++)
            {
                recv[n] = 0;
            }
            if ((n = recvfrom(sockfd, recv, 1000, 0, (struct sockaddr*) NULL, NULL)) < 0)
            {
                perror(NULL);
                close(sockfd);
                exit(1);
            }

            printf("%s", recv);
        }
    }

    return 0;
}
