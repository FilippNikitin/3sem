#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main()
{
    int msqid;
    char pathname[] = "klient.c";
    key_t key;
    int id = getpid();

    struct inquiry
    {
        int a;
        int b;
    };

    struct mymsgbufout
    {
        long mtype;
        struct inquiry data;
    } mybufout;

    struct mymsgbufin
    {
        long mtype;
        long int result;
    } mybufin;

    mybufin.result = 0;
    if ((key = ftok(pathname, 0)) < 0)
    {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0)
    {
        printf("Can\'t get msqid\n");
        exit(-1);
    }

    scanf("%d %d", &(mybufout.data.a), &(mybufout. data.b));
    mybufout.mtype = (long)id; //getpid();

    if (msgsnd(msqid, (struct msgbufout *) &mybufout, sizeof(struct inquiry), 0) < 0)
    {
        printf("Can\'t send message to queue\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);
        exit(-1);
    }

    if ((msgrcv(msqid, (struct msgbufin *)&mybufin, sizeof(long int), id/*getpid()*/, 0)) < 0)
    {
        printf("Can\'t receive message from queue\n");
        exit(-1);
    }

    printf("%ld\n", mybufin.result);
    return 0;
}
