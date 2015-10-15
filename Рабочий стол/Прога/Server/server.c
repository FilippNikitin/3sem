#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/sem.h>
#include <unistd.h>
#define N 1
#define FileName "klient.c"

/*
    Здесь много повторяющегося кода, если надо, разобью на функции
    V: Да, избавьтесь от дублирования кода и засчитаем.
*/
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

int semid;
int msqid;

void* ProcessInquiry(void* arg);
key_t GenerateKey();
int CreateSemaphore();
void ChangeSemaphore(int semid, int n);
int CreateQueue();

int main()
{
    struct mymsgbufin mybufin;
    pthread_t thread_id;
    semid = CreateSemaphore();
    msqid = CreateQueue();
    ChangeSemaphore(semid, N);

    while (1)
    {
        if ((msgrcv(msqid, (struct msgbufin *)&mybufin, sizeof(struct inquiry), 0, 0)) < 0)
        {
            printf("Can\'t receive message from queue\n");
            exit(-1);
        }
        pthread_create(&thread_id, (pthread_attr_t *)NULL , ProcessInquiry, &mybufin);
    }
    return 0;
}

void* ProcessInquiry(void* arg)
{
    struct mymsgbufin mybufin = *((struct mymsgbufin *)arg);
    struct mymsgbufout mybufout;

    ChangeSemaphore(semid, -1);
    mybufout.result = mybufin.data.a * mybufin.data.b;
    mybufout.mtype = mybufin.mtype;

    if (msgsnd(msqid, (struct msgbufout *) &mybufout, sizeof(long int), 0) < 0)
    {
        printf("Can\'t send message to queue\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);
        exit(-1);
    }
    sleep(30);
    ChangeSemaphore(semid, 1);
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

int CreateSemaphore()
{
    key_t key = GenerateKey();
    int semid;
    struct sembuf mybuf;
    if ((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0)
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

void ChangeSemaphore(int semid, int n)
{
    struct sembuf mybuf;
    mybuf.sem_op = n;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;
    if (semop(semid, &mybuf, 1) < 0)
    {
        printf("Can`t wait for condition\n");
        exit(-1);
    }
}
