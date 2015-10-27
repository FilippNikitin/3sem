#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/sem.h>
#include <unistd.h>
#define N 4
#define FileName "klient.c"

struct inquiry
{
    int a;
    int b;
};

struct mymsgbufin
{
    long mtype;
    struct inquiry data;
};

struct mymsgbufout
{
    long mtype;
    long int result;
};

int semid, semidNew;
int msqid;

void* ProcessInquiry(void* arg);
key_t GenerateKey();
int CreateSemaphore(int n);
void ChangeSemaphore(int semid, int i, int n);
int CreateQueue();

int main()
{
    struct mymsgbufin mybufin;
    pthread_t thread_id;
    semid = CreateSemaphore(2);
    msqid = CreateQueue();
    ChangeSemaphore(semid, 0, N);

    while (1)
    {
        if ((msgrcv(msqid, (struct msgbufin *)&mybufin, sizeof(struct inquiry), 0, 0)) < 0)
        {
            printf("Can\'t receive message from queue\n");
            exit(-1);
        }
        pthread_create(&thread_id, (pthread_attr_t *)NULL , ProcessInquiry, &mybufin);
        ChangeSemaphore(semid, 1, -1);
    }
    return 0;
}

void* ProcessInquiry(void* arg)
{
   /*
    * FIXIT:
    * Я не доглядел одно слабое месте. Может случиться так, что следующее сообщение придёт раньше, чем вы скопируете arg в локальную переменную.
    * С помощью дополнительного семафора необходимо гарантировать, что до тех пор, пока это копирование не завершится, вы не будете считывать следующее сообщение из очереди.
    */

    struct mymsgbufin mybufin = *((struct mymsgbufin *)arg);
    sleep(10);
    ChangeSemaphore(semid, 1, 1);
    struct mymsgbufout mybufout;

    ChangeSemaphore(semid, 0, -1);
    mybufout.result = mybufin.data.a * mybufin.data.b;
    mybufout.mtype = mybufin.mtype;

    sleep(30);

    if (msgsnd(msqid, (struct msgbufout *) &mybufout, sizeof(long int), 0) < 0)
    {
        printf("Can\'t send message to queue\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);
        exit(-1);
    }
    ChangeSemaphore(semid, 0, 1);
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

int CreateQueue()
{
    key_t key = GenerateKey();
    int msqid;
    struct mymsgbufin mybufin;
    struct mymsgbufout mybufout;
    if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0)
    {
        printf("Can\'t get msqid\n");
        exit(-1);
    }
    return msqid;
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
