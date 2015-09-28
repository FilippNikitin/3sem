#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

/*
 * FIXIT: 
 * Не ставьте пустые строки между каждой парой. 
 */


//константы*********************************************************************************************************

const int max_task_num = 100;

const int max_task_len = 100;

const int max_arg_len = 10;

const int max_arg_num = 100;


//******************************************************************************************************************


//для удобства использования execvp храним задачи в виде след. структуры

/*
 * FIXIT:
 * Я полностью с вами согласен. Думаю согласятся многие. Не нужно это комментировать.
 * 
 * Вы называете ф-ю complete_tasks - завершить задачи. видимо нужен другой глагол.
 * И не нужно переводить название в комментарии. Понятного названия на английском вполне достаточно.
 */


typedef struct task
  {
    char * cmd;

    char ** cmd_arg;

    int num_arg;

    /*
     * FIXED:
     * Может лучше sleepTime? - время сна.
     * Спросите, например, у Ильи К. про стили написания кода. И вас помесь нескольких. Нужно выбрать какой-то один.
     * Пусть он вам до семинара прокомментирует какие они бывают (пусть это будет C# стиль).
     * Если до семинара не успеете, то я на паре вам расскажу как надо.
     */
    int time_sleep; 

   } task;


// Функции **************************************************************************************************


 
void Split(char *string, char *delimiters, char ***tokens, int *tokens_count);//Ф-я разделяющая строки на слова

void get_task(FILE * fin, task ** tasks, int num); // Чтение и преобразование данных из т. файла

void complete_tasks(task** tasks, int num); //Выполнение комманд


//Тело программы*********************************************************************************************


int main()
{
	int i, cmd_num;        

        task *tasks;

	FILE *fin = fopen("input.txt", "r");

	char *c= (char *)malloc(max_task_len * sizeof(char)+1);

	fgets(c, max_task_len, fin);

	cmd_num = atoi(c);

	tasks = (task *) malloc(max_task_num * sizeof(task)+1);

	get_task(fin, &(tasks), cmd_num);

	complete_tasks(&tasks, cmd_num);

        for (i = 0; i < cmd_num; i++)

	     free((tasks+i)->cmd_arg);

	free(tasks);

	free(c);

	fclose(fin);

	return 0;
}



//Описание функций ********************************************************************************************************



void Split(char *string, char *delimiters, char ***tokens, int *tokens_count)//воспользуемся ф-ей с вторго семинара
{
	char *token;

        int token_size = 0, next_size = 4;

        int counter = 0;

        char ** token_local = *tokens;

	for (token = strtok(string, delimiters); token != NULL;

	     token = strtok(NULL, delimiters)) {

		if (counter == token_size) {

			token_size = next_size;
                        
                        token_local = (char **)realloc(token_local, token_size * sizeof(char *)+1);

			next_size *= 2;
		}

		token_local[counter++] = token;
	}

	*tokens = token_local;

	*tokens_count = counter;
}


//********************************************************************************************************

void get_task(FILE * file, task ** tasks, int num)
{
	int i, k;

	char* delimiters=" \n ";//разделители//

	char ** pointers_arg = (char **)malloc((max_arg_num + 1) * sizeof(char *)+1);//двумерный массив аргументов 

	for (i = 0; i < num; i++) 
               {
		(*(tasks)+i)->cmd_arg = (char **)malloc(max_arg_num * sizeof(char)); //выделение памяти для массива задач

                char* local_task =   (char *)malloc(max_task_len * sizeof(char));//выделение памяти для строки, хранящей инф о задаче внутри функции
		fgets(local_task, max_task_len, file);//получим нужную нам строку из файла

		Split(local_task, delimiters, &(pointers_arg),&(((*tasks)+i)->num_arg));//разделим её на "слова"

    /*
     * FIXIT:
     * Думаю, так понятнее.
      (*tasks)[i].time_sleep
      и не комментируйте то, что и так понятно. 
    */
    
		((*tasks)+i)->time_sleep = atoi(pointers_arg[0]);// запишем время в структуру

		((*tasks)+i)->cmd = pointers_arg[1];//команду

		((*tasks)+i)->num_arg=((*tasks)+i)->num_arg-1;//кол-во аргументов

		for (k = 0; k < ((*tasks)+i)->num_arg; k++)//аргументы

                     (((*tasks)+i)->cmd_arg)[k] = pointers_arg[k + 1];

		(((*tasks)+i)->cmd_arg)[k + 1] = NULL;

	}
	free(pointers_arg);//избавимся от "временной" памяти

}

//******************************************************************************************************

void complete_tasks(task ** tasks, int num)
{
	pid_t pid=0;

	int i, id;

	for (i = 0; i < num; ++i)//чтобы нам запустить n программ, нужно n процессов 
              { 

		pid = fork();//форкаемся

		if (pid == 0) 
                      {	

			sleep(((*tasks)+i)->time_sleep);//ждём

			execvp(((*tasks)+i)->cmd, ((*tasks)+i)->cmd_arg);//подменяем дочерний процесс

			printf("Error\n");//что-то пошло не так:)

			exit(-1);
		}
	}
	for (i =0; i < num; i++)

		pid = wait(&id);//ждём завершения процессов
}
