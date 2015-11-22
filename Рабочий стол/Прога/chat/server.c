#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

/*
 * Засчитано, но поправте ряд формальных замечаний. 
 * Желательно уже просто свыкнуться с этими вещами и сразу делать так.
 */

/*
 * Совет: называйте структуры с заглавной буквы. Тогда не надо будет думать над названием переменной: User user;
 */
struct user
{
    char* nick;
    struct sockaddr_in usersInf;
};

char *MakeAnswer(char* nick, char* msg);

int main()
{
    /*
     * Отдельные константы для 1000, 53000, 999
     */
    int sockfd;
    int clilen, n;
    char *line = (char*)malloc(1000);
    struct user *usersList = NULL;
    int numUsers = 0;
    struct sockaddr_in servaddr, cliaddr;

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(53000);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror(NULL);
        exit(1);
    }
    else
    {
        printf("Sock fd:%d\n", sockfd);
    }

    if (bind(sockfd, (struct sockaddr*) &servaddr,
        sizeof(servaddr)) < 0)
    {
        perror(NULL);
        close(sockfd);
        exit(1);
    }

    while (1)
    {
        int i;
	/*
	 * bzero для обнуления
	 */
        for (i = 0; i < 1000; i++)
            line[i] = 0;

        clilen = sizeof(cliaddr);
        if ((n = recvfrom(sockfd, line, 999, 0,
            (struct sockaddr*)&cliaddr, &clilen)) < 0)
        {
            perror(NULL);
            close(sockfd);
            exit(1);
        }

        /*
	 * Нужны более ясные названия:
	 * isUserFound или isNewUser .. что-то более осмысленное.
	 * userIndex.
	 * 
	 * Лучше каждое такое действие, вроде проверки наличия пользователя в списке данного чата выносить в отдельные ф-я, которые называть
	 * соответсвующим образом. Это сделает код самодокументируемым. Если вы будете работать в команде, это сэкономит время вашим напарникам
	 * в понимании кода, и все вам будут только благодарны. 
	 */
        int f = 0;
        int nUser;

        for (i = 0; i < numUsers; i++)
        {
            if (((usersList[i].usersInf.sin_addr.s_addr) == (cliaddr.sin_addr.s_addr))&&
                ((usersList[i].usersInf.sin_port) == (cliaddr.sin_port)))
            {
                f = 1;
                nUser = i;
                break;
            }

        }

        if (f == 0)
        {
            usersList = (struct user *)realloc(usersList, (numUsers + 1) * sizeof(struct user));
            (usersList[numUsers]).nick = (char *)malloc(strlen(line) * sizeof(char) + 1);
            (usersList[numUsers]).nick = strcpy((usersList[numUsers]).nick, line);
            (usersList[numUsers]).usersInf = cliaddr;
            numUsers++;
            printf("New user :%s\n", line);
            line = strcat(line," join.\n" );
            char a[] = "Admin";
            line = MakeAnswer(a,line);
        }
        else
        {
            line = MakeAnswer((usersList[nUser]).nick, line);
        }

        for (i = 0; i < numUsers; i++)
        {
            if (sendto(sockfd, line, strlen(line), 0,
                (struct sockaddr*)&(usersList[i]).usersInf , clilen) < 0)
            {
                perror(NULL);
                close(sockfd);
                exit(1);
            }
        }
    }
    return 0;
}

char *MakeAnswer(char* nick, char* msg)
{
    char *tmp = (char*)malloc(1000);
    tmp = strcpy(tmp, nick);
    tmp = strcat(tmp, ": ");
    tmp = strcat(tmp, msg);
    const time_t timer = time(NULL);
    char *time = malloc(sizeof(char)*35);
    time = ctime(&timer);
    tmp = strcat(tmp, time);
    msg = strcpy(msg, tmp);
    return(msg);
    free(time);
    free(tmp);
}
