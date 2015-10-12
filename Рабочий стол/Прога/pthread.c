#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#define mytype int
#define N 1000
#define N1 1
/*
    Я попытался распараллелить умножение матриц.
    Однако эффекта ускорения не получил.
*/

mytype **matrix1;
mytype **matrix2;
mytype **resultMyliply;

int i = 0;
int factor = N/N1;

void CreateMatrix(mytype **m, int n);
mytype** GetMemoryMatrix(mytype **a, int n);
void** FreeMemoryMatrix(mytype **a, int n);
void* my_thread(void* dummy);
void PrintMatrix(mytype **m, int n);


int main()
{
    pthread_t *thread_id;
    clock_t time;

    thread_id =  (pthread_t*) malloc(N1 * sizeof(pthread_t));
    int result;
    int n = N;
    int j;
    matrix1 = GetMemoryMatrix(matrix1, n);
    CreateMatrix(matrix1, n);
    matrix2 = GetMemoryMatrix(matrix2, n);
    CreateMatrix(matrix2, n);
    resultMyliply = GetMemoryMatrix(resultMyliply, n);
    time = clock();
    for (j = 0; j < N1; j++)
    {
        result = pthread_create(&thread_id[j] , (pthread_attr_t *)NULL , my_thread , NULL);
    }


    for (j = 0; j < N1; j++)
    {
        pthread_join(thread_id[j] , (void **) NULL);
    }
    time = (clock() - time)/CLOCKS_PER_SEC;
    printf("%d", time);
    FreeMemoryMatrix(matrix1, n);
    FreeMemoryMatrix(matrix2, n);
    FreeMemoryMatrix(resultMyliply, n);
    return 0;
}


void CreateMatrix(mytype **m, int n)
{
    int l, j;
    for(l = 0; l < n; l++)
    {
        for(j = 0; j < n; j++)
        {
             m[l][j] = (int) rand()%N;
        }
    }
}

mytype** GetMemoryMatrix(mytype **a, int n)
{
    int l;
    a = (mytype **)malloc(n * sizeof(mytype *));
    for(l = 0; l < n; l++)
    {
        a[l] = (mytype *) malloc(n * sizeof(mytype));
    }
    return a;
}

void** FreeMemoryMatrix(mytype **a, int n)
{
    int l;
    for(l = 0; l < n; l++)
    {
        free(a[l]);
    }

    free(a);
}

void PrintMatrix(mytype **m, int n)
{
    int i, j;
    for(i = 0; i < n; i++)
    {
        for(j=0;j<n;j++)
        {
            printf("%d ",m[i][j]);
        }
        printf("\n");
    }
}

void* my_thread(void* dummy)
 {
    int j, k, l, m;
    if (i < N1)
    {
        j = i;
        i++;
    }
    pthread_t my_thread_id;
    for (m = j*factor; m < (j + 1)*factor; m++)
    {
    if (m < N)
    {
    for (k = 0; k < N; k++)
    {
        for(l = 0; l < N; l++)
        {
            resultMyliply[m][k] += matrix1[m][l]*matrix2[l][k];
        }
    }
    }
    }
    return NULL;
}






































