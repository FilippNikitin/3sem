#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>

void main(void)
{
	int n, i;
	int status = 0;
	scanf("%d", &n);
	for (i = 0; i < n; ++i)
	{
		pid_t pid = fork();
		if (pid == 0)
		{
			exit(i);
		}
	}
	for (i = 0; i < n; ++i)
	{
		pid_t id = wait(&status);
		printf("id: %d\n", id);
		printf("status: %d\n", status);
	}}
