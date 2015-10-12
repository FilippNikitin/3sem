#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define N 10000000

int a[3] = {0, 0, 0};
const char pathname[] = "seminar5_race.c";

void* my_thread(void* dummy)
{

    struct sembuf mybuf;
    int i;
    int semid;
    key_t key;
    if ((key = ftok(pathname, 0)) < 0)
    {
        printf("Can`t generate key\n");
        exit(-1);
    }
    if ((semid = semget(key, 1, 0666)) < 0)
    {
        printf("Can`t get semid\n");
        exit(-1);
    }
    pthread_t my_thread_id;
    my_thread_id = pthread_self();
    for (i = 0; i < N; i++)
    {
        a[1]++;
        a[0]++;
	}
    printf("id%d a[0]%d, a[1]%d\n", my_thread_id, a[0], a[1]);
    mybuf.sem_op =  1;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;
    if (semop(semid , &mybuf , 1) < 0)
    {
        printf("Can`t wait for condition\n");
        exit(-1);
    }
    return NULL;
}


int main()

{
    key_t key;
    struct sembuf mybuf;
    int semid;
    if ((key = ftok(pathname, 0)) < 0 )
    {
        printf("Can`t generate key\n");
        exit(-1);
    }
    if ((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0)
    {
        printf("Can`t get semid\n");
        exit(-1);
    }
    pthread_t thread_id, my_thread_id;
    int result;
    result = pthread_create(&thread_id, (pthread_attr_t *)NULL, my_thread, NULL);
    if (result)
    {
        printf("Can`t create thread, returned value = %d\n" , result);
        exit(-1);
    }
    my_thread_id = pthread_self();
    mybuf.sem_op = -1;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;
    if (semop(semid , &mybuf , 1) < 0)
    {
        printf("Can`t wait for condition\n");
        exit(-1);
    }
    int i;
    for (i = 0; i < N; i++)
    {
      /*
       * Я не очень хорошо сформулировал задание. В итоге, у вас получился фактически последовательный код.
       * Можно было бы с помощью семафора "окружить" только критическую секцию, т.е.
       * a[1]++;
       * это глобальная переменная, которая инкрементируется в обоих потоках.
       * 
       * Можно было бы сделать так:
       * for ...
       * {
       *    ...
       *    D(S, 1)
       *    a[1]++;
       *    A(S, 1)
       * }
       * Засчитано.
       */
        a[1]++;
        a[2]++;
	}
    printf("id%d a[1]%d, a[2]%d\n" , my_thread_id, a[1], a[2]);
    pthread_join(thread_id , (void **) NULL);
    semctl(semid, IPC_RMID, NULL);
    return 0;
}






































