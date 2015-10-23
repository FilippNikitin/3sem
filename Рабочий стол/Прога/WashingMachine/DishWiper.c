#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>

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

key_t GenerateKey();
int CreateSemaphore();
void ChangeSemaphore(int semid, int n);
int CreateQueue();

int msqid;
int semid;

int main()
{
    int msqid;
    key_t key;
    struct mymsgbuf mybufin;

    key = GenerateKey();
    msqid = CreateQueue();
    semid = CreateSemaphore();

    while (1)
    {

        if ((msgrcv(msqid, (struct msgbuf *)&mybufin, sizeof(struct dish),2 , 0)) < 0)
        {
            printf("Can\'t receive message from queue\n");
            exit(-1);
        }
        printf("Dish is wiping: size %d, amount of water %d\n", mybufin.cleanDish.size, mybufin.cleanDish.amountOfWater );
        sleep( mybufin.cleanDish.size + mybufin.cleanDish.amountOfWater );
        printf("Dish was wiped\n");
        ChangeSemaphore(semid, mybufin.cleanDish.size);
    }
    return 0;
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

