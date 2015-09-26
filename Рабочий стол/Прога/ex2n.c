#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>

/*
 * FIXIT:
 * форматирование никуда не годится: отступы, пробелы вокруг бинарных операторов,
 * спросите у тех, кто сдал первое упражнение про split, как надо оформлять код.
 */

/*
 * Пусть каждый процесс выведет, помимо своего id ещё и родительский, чтобы была видна родственная связь.
 * Вероятно получится сделать так, чтобы цикл был до n - 1, и не было дополнительного fork до цикла, тогда код вероятно будет понятнее и короче.
 */

/* 
*FIXED
*Попробовал форматировать несколько по-другому (готов совершенствоваться в этом плане)
*Добавил отображение родственной связи
*Избавился от forka до цикла, но это не сократил программу
*/

int main()
{
  int n , i;
  int status = 0;
  pid_t firstid = getpid(), id, pid;
  printf("I burn (c)%d\n", firstid);
  printf("How many processes ");
  scanf("%d",&n);
  sleep(1);
  if (n == 1)
     {
      printf("Process die(c)%d\n", getpid());                                     
      exit(0); 
     }
  for (i = 0; i < n - 1 ; i++)
     {
      if ((pid == 0)|(i == 0))//мы в дочернем процессе?
         {     
          if (getpid() != firstid)
             {
              printf("Process №%d id%d created\n",i + 1, getpid());
              printf("My parent id%d\n", getppid());
              sleep(1);
             }
          pid = fork();//форкаемся	
         }
      else //мы в род. процессе
         { 
          id = wait(&status);//ждём завершения дочернего процесса
          if (getpid() == firstid)
             {                       
              printf("Process №%d id%d completed\n", i + 1, id);//сообщаем о завершение дочернего процесса	
              sleep(1);
              printf("Process die(c)%d\n", getpid());
              exit(0);
             }
          printf("Process №%d id%d completed\n", i + 2, id);//сообщаем о завершение дочернего процесса
	  sleep(1);
          exit(0);//завершаемся
         }
      }
  if (pid == 0)//если мы в дочернем (последнем)
     { 
      printf("Process №%d id%d created\n", n, getpid());
      printf("My parent id%d\n", getppid());
      sleep(1);
      exit(0);
     }
  if (pid != 0)//если мы в предпоследнем
     {
      id = wait(&status);//ждём завершения последнего
      printf("Process №%d id%d completed\n", n, id);
      sleep(1);
      exit(0);
     }
}
