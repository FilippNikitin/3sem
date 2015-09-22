#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
	int n , i;

	int status = 0;
        
        pid_t id, firstid = getpid();
	
	printf("I burn (c)%d\n", firstid);

	scanf("How many processes%d", &n);
	
	sleep(1);
	

        pid_t pid = fork();//форкаемся первый раз, чтобы процесс запустился в цикле 

	for (i = 0; i < n - 2 ; ++i){

		if (pid == 0)//мы в дочернем процессе?

		{     		      
                      printf("Process №%d id %d created\n",i+2, getpid());

		      sleep(1);

		      pid = fork();//форкаемся	
		}
		else //мы в род. процессе
		{ 
		       id = wait(&status);//ждём завершения дочернего процесса
			
		       if (getpid() == firstid)
				{                       
                                     printf("Process  №%d id%d completed\n",i+2,id);//сообщаем о завершение дочернего процесса
	
                                     sleep(1);

                                     printf("Process die(c)%d\n", getpid());
                                     
                                     exit(0);
                                  }
                              		       

                       printf("Process №%d id%d completed\n",i+2,id);//сообщаем о завершение дочернего процесса
	
                       sleep(1);
			
                       exit(0);//завершаемся
		}
	}

	if (pid == 0)//если мы в дочернем (последнем)
	{ 
	              printf("Process №%d id%d created\n",n,getpid());
                      
                      sleep(1);
			
	              exit(0);

	}
 	if (pid!=0)//если мы в предпоследнем
	{
	            id=wait(&status);//ждём завершения последнего

                    printf("Process №%d id%d completed\n",n,id);

                    sleep(1);

                    exit(0);
	}
}
