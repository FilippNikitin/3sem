#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define N 4
#define FileName "key.txt"
#define output 1
#define input 0


char * name1 = "out.fifo";
char * name2 = "in.fifo";


struct dish
{
	int size;
	int DirtinessRate;
};

struct dish_c
{
	int size;
	int amountOfWater;
};

struct mymsgbuf
{
    long mtype;
    struct dish_c cleanDish;
};

int semid;
int msqid;

key_t GenerateKey();
int CreateSemaphore();
void ChangeSemaphore(int semid, int n);
int CreateQueue();
void CreateFifo (char *name);
int OpenFifo(char *name, int flag);
struct dish GetDish(int *fdin);
void AnswerGen(int *fdout, int flag);
struct dish_c WashDish(struct dish dirtyDish);
void PutOnTable(struct dish_c cleanDish);

int main()
{
    int fdin, fdout;
	int answer = 1;
    struct dish dirtyDish;
    struct dish_c cleanDish;

    while(1)
    {
        dirtyDish = GetDish(&fdin);
        printf("Dirty dish is got: size %d, dirtiness rate %d\n", dirtyDish.size, dirtyDish.DirtinessRate);
        cleanDish = WashDish(dirtyDish);
        printf("Dish was washed:  size %d, amount of water %d \n", cleanDish.size, cleanDish.amountOfWater);
        PutOnTable(cleanDish);
        AnswerGen(&fdout, answer);
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

int CreateSemaphore()
{
    key_t key = GenerateKey();
    int semid;
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

/*
 * Повторяющийся в разных *.c файлах код можно вынести в отдельный *.h файл
 * Потом компилировать так: gcc file.c file.h -o executefile
 */

void CreateFifo (char *name)
{
    umask(0);
	if (mknod(name , S_IFIFO | 0666 , 0) < 0)
	{
       	printf("Can`t create FIFO\n");
       	exit(-1);
	}
}

int OpenFifo(char *name, int flag)
{
    int fd;
    if (flag)
    {
        if  ((fd = open(name, O_WRONLY)) < 0)
        {
            printf("Can`t open FIFO");
            exit(-1);
        }
    }
    else
    {
        if ( (fd = open(name, O_RDONLY)) < 0)
        {
            printf("Can`t open FIFO");
            exit(-1);
        }
    }
    return fd;
}

struct dish GetDish(int *fdin)
{
    struct dish dirtyDish;
    *fdin = OpenFifo(name1, input);
    read(*fdin, &dirtyDish, sizeof(struct dish));
    close(*fdin);
    return dirtyDish;
}

void AnswerGen(int *fdout, int flag)
{

    *fdout = OpenFifo(name2, output);
    write(*fdout, &flag, sizeof(int));
    close(*fdout);
}

struct dish_c WashDish(struct dish dirtyDish)
{
    struct dish_c clearDish;
    printf("Dish is washing\n");
    sleep(dirtyDish.DirtinessRate + dirtyDish.size);
    clearDish.amountOfWater = (int) rand()%15;
    clearDish.size = dirtyDish.size;
    return clearDish;
}

void PutOnTable(struct dish_c cleanDish)
{
   /*
    * Сейчас, конечно, не особо важно, но в случае учёта производительности, довольно накладно для каждой тарелки вызывать semget, msgget.
    * Это достаточно один раз сделать.
    */
   
    semid = CreateSemaphore();
    msqid = CreateQueue();
    ChangeSemaphore(semid, (cleanDish.size)*(-1));
    struct mymsgbuf mybufout;
    mybufout.cleanDish = cleanDish;
    mybufout.mtype = 1;

    if (msgsnd(msqid, (struct msgbuf *) &mybufout, sizeof(struct dish_c), 0) < 0)
    {
        printf("Can\'t send message to queue\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);
        exit(-1);
    }
}
