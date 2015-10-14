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

/*
    Здесь много повторяющегося кода, если надо, разобью на функции

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

void* my_thread(void* arg);

int main()
{
    char pathname[] = "klient.c";
    key_t key;

    int msqid;
    struct mymsgbufin mybufin;
    struct mymsgbufout mybufout;

    int semid;
    struct sembuf mybuf;

    pthread_t thread_id;

    if ((key = ftok(pathname, 0)) < 0)
    {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if ((semid = semget(key, 1, 0666| IPC_CREAT)) < 0)
    {
        printf("Can`t get semid\n");
        exit(-1);
    }

    mybuf.sem_op = N;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;

    if (semop(semid , &mybuf , 1) < 0)
    {
        printf("Can`t wait for condition\n");
        exit(-1);
    }

    if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0)
    {
        printf("Can\'t get msqid\n");
        exit(-1);
    }

    while (1)
    {
        if ((msgrcv(msqid, (struct msgbufin *)&mybufin, sizeof(struct inquiry), 0, 0)) < 0)
        {
            printf("Can\'t receive message from queue\n");
            exit(-1);
        }
        pthread_create(&thread_id, (pthread_attr_t *)NULL , my_thread, &mybufin);
    }
    return 0;
}

void* my_thread(void* arg)
{
    char pathname[] = "klient.c";
    key_t key;

    int msqid;
    struct mymsgbufin mybufin;
    struct mymsgbufout mybufout;
    mybufin = *((struct mymsgbufin *)arg);

    int semid;
    struct sembuf mybuf;

    if ((key = ftok(pathname, 0)) < 0)
    {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if ((semid = semget(key, 1, 0666)) < 0)
    {
        printf("Can`t get semid\n");
        exit(-1);
    }

    mybuf.sem_op = -1;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;

    if (semop(semid , &mybuf , 1) < 0)
    {
        printf("Can`t wait for condition\n");
        exit(-1);
    }


    if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0)
    {
        printf("Can\'t get msqid\n");
        exit(-1);
    }

    mybufout.result = mybufin.data.a * mybufin.data.b;
    mybufout.mtype = mybufin.mtype;

    if (msgsnd(msqid, (struct msgbufout *) &mybufout, sizeof(long long int), 0) < 0)
    {
        printf("Can\'t send message to queue\n");
	msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);
	exit(-1);
    }

    mybuf.sem_op = 1;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;

    if (semop(semid , &mybuf , 1) < 0)
    {
        printf("Can`t wait for condition\n");
        exit(-1);
    }

}
