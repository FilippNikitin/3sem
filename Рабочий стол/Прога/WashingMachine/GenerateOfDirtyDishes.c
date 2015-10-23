#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 10
#define output 1
#define input 0


const char * name1 = "out.fifo";
const char * name2 = "in.fifo";


void CreateFifo (const char *name);
int OpenFifo(const char *name, int flag);

int main ()
 {
	struct dish
	{
		int size;
		int DirtinessRate;
	}dirtyDish;

	int fdin, fdout;
	int answer = 1;
    CreateFifo(name1);
    CreateFifo(name2);

	while ((1)&&(answer))
	{
        fdout = OpenFifo(name1, output);
		dirtyDish.size = (int) rand() % N+1;
		dirtyDish.DirtinessRate = (int) rand() % N;
		printf("Dirty dish: size %d, dirtiness rate %d\n", dirtyDish.size, dirtyDish.DirtinessRate);
		write(fdout, &dirtyDish, sizeof(struct dish));
		close(fdout);
        fdin = OpenFifo(name2, input);
		read(fdin, &answer, sizeof(int));
		close(fdin);

	}
	return 0;
}

void CreateFifo (const char *name)
{
    umask(0);
	if (mknod(name , S_IFIFO | 0666 , 0) < 0)
	{
       	printf("Can`t create FIFO\n");
       	exit(-1);
	}
}

int OpenFifo(const char *name, int flag)
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
