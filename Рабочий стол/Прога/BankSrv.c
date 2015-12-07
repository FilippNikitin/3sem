#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/sem.h>

const char *positive = "Operation have had success\n\0";
const char *negative = "There aren't enouth money in cashpoint\n\0";

#define FileName "key.txt"

const int MaxRequest = 512;
const int MaxResponse = 512;
const int Port = 52000;
const int MaxDepth = 5;
const int Money = 100500;

int semid;
int balance;

struct inquary
{
    struct sockaddr_in usraddr;
    char *request;
    int sockfd;
};

void* ProcessInquiry(void* arg);
key_t GenerateKey();
int CreateSemaphore(int n);
void ChangeSemaphore(int semid, int i, int n);

int main()
{
	int sockfd, newsockfd;
	int clilen;
	int n;
	struct sockaddr_in servaddr;

	balance = Money;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror(NULL);
		exit(1);
	}

	bzero(&servaddr, sizeof servaddr);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(Port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof servaddr) < 0)
	{
		perror(NULL);
		close(sockfd);
		exit(1);
	}

	if (listen(sockfd, MaxDepth) < 0)
	{
		perror(NULL);
		close(sockfd);
		exit(1);
	}

    semid = CreateSemaphore(2);
    ChangeSemaphore(semid, 0, 1);
    ChangeSemaphore(semid, 1, 1);

	while(1)
	{
        struct inquary tmp;
		clilen = sizeof(tmp.usraddr);
		if ((tmp.sockfd = accept(sockfd, (struct sockaddr *)&(tmp.usraddr), &clilen)) < 0)
		{
				perror(NULL);
				close(sockfd);
				exit(1);
		}
        pthread_t thread_id;
    	ChangeSemaphore(semid, 1, -1);
        pthread_create(&thread_id, (pthread_attr_t *)NULL , ProcessInquiry, &tmp);
	/*
	 * FIXIT:
	 * нет никакой гарантии, что данные из структуры
	 * struct inquary tmp;
	 * будут скопированы раньше, чем она будет удалена со стека.
	 * 
	 * Ровна та же проблемы была у вас в клиент/сервере с очередью сообщений.
	 */
	}
	return 0;
}
void* ProcessInquiry(void* arg)
{
    int n, usrMoney;
    struct inquary usr = *((struct inquary *)arg);
    usr.request = (char*)malloc(MaxRequest*sizeof(char));
    ChangeSemaphore(semid, 1, 1);
    while((n = read(usr.sockfd, usr.request, MaxRequest)) > 0)
    {
        ChangeSemaphore(semid, 0, -1);
        sleep(15);
	/*
	 * Мне кажется, лучше сделать числовую переменную 
	  int request = atoi(usr.request); 
	 */
	int request = atoi(usr.request); 
        if(request > 0)
        {
            balance += request;
            printf("%d\n", balance);
            strcpy(usr.request, positive);
            printf("1*%s\n", usr.request);
        }
        else
        {
            if ((balance + request) >= 0)
            {
                balance += atoi(usr.request);
                strcpy(usr.request, positive);
            }
            else
            {
                strcpy(usr.request, negative);
            }
        }
        ChangeSemaphore(semid, 0, 1);
        if((n = write(usr.sockfd, usr.request, MaxResponse)) < 0)
        {
            perror(NULL);
            close(usr.sockfd);
            exit(1);
        }
        close(usr.sockfd);
    }
}

key_t GenerateKey()
{
    key_t key;
    char pathname[] = FileName;
    if ((key = ftok(pathname, 0)) < 0)
    {
        printf("Can\'t generate key\n");
        exit(-1);
    }
    return key;
}

int CreateSemaphore(int n)
{
    key_t key = GenerateKey();
    int semid;
    if ((semid = semget(key, n, 0666 | IPC_CREAT)) < 0)
    {
        printf("Can`t get semid\n");
        exit(-1);
    }
    return semid;
}


void ChangeSemaphore(int semid, int i, int n)
{
    struct sembuf mybuf;
    mybuf.sem_op = n;
    mybuf.sem_flg = 0;
    mybuf.sem_num = i;
    if (semop(semid, &mybuf, 1) < 0)
    {
        printf("Can`t wait for condition\n");
        exit(-1);
    }
}
