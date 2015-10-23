#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/sem.h>
#include <unistd.h>

#define size_of_table 15
#define FileName "key.txt"

struct  dish
{
    int size;
    int amountOfWater;
};

struct mymsgbuf
{
    long mtype;
    struct dish cleanDish;
};

int semid;
int msqid;

void* ProcessInquiry(void* arg);
key_t GenerateKey();
int CreateSemaphore();
void ChangeSemaphore(int semid, int n);
int CreateQueue();
void* WaitingOnTable(void* arg);
struct mymsgbuf GetDish();

int main()
{
    struct mymsgbuf mybufin;
    pthread_t thread_id;
    semid = CreateSemaphore();
    ChangeSemaphore(semid, size_of_table);

    while (1)
    {
        mybufin = GetDish();
        printf("Dish is on the table: size %d, amount of water %d\n",
               mybufin.cleanDish.size, mybufin.cleanDish.amountOfWater);

        pthread_create(&thread_id, (pthread_attr_t *)NULL , WaitingOnTable, &mybufin);
    }
    return 0;
}

struct mymsgbuf GetDish()
{
    msqid = CreateQueue();
    struct mymsgbuf mybufin;

    if ((msgrcv(msqid, (struct mymsgbuf *)&mybufin, sizeof(struct dish), 1, 0)) < 0)
    {
        printf("Can\'t receive message from queue\n");
        exit(-1);
    }
    return mybufin;
}

void* WaitingOnTable(void* arg)
{
    struct mymsgbuf mybuf = *((struct mymsgbuf *)arg);
    mybuf.mtype = 2;

    if (msgsnd(msqid, (struct msgbuf *) &mybuf, sizeof(struct dish), 0) < 0)
    {
        printf("Can\'t send message to queue\n");
        exit(-1);
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
