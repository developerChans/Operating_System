/*
*   DKU Operating System Lab
*           Lab2 (Vehicle production Problem)
*           Student id : 32191890
*           Student name : ParkChanJin(developerChans)
*
*   lab2_thread_example.c :
*       - thread usage example code.
*       - thread argument example code.
*
*   Implement thread argument code 
*/

#include <aio.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <asm/unistd.h>

#include "lab2_sync_types.h"

typedef struct MultipleArg {
	int _a;
	int _b;
}MA;

void *t_func(void *multi_arg) {
	MA *my_multi_arg = (MA *)multi_arg;
	printf("a: %d, b: %d\n", my_multi_arg->_a, my_multi_arg->_b);
}

int main(int argc, char* argv[]) {
	pthread_t t1, t2;

	MA *multiple_arg;
	multiple_arg = (MA *)malloc(sizeof(MA));
	multiple_arg->_a = 10; multiple_arg->_b = 20;

	pthread_create(&t1, NULL, t_func, (void *) multiple_arg);
	pthread_create(&t2, NULL, t_func, (void *) multiple_arg);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	return 0;
}




typedef struct MultipleArgpro {
	car *car_arr;
	cq *result
	int n
	int timequantum
}MAP;


void RR(void *multproduccer)
{
	MAP *pro_arg = (MAP *)multiproduccer;
	cq q;
	car* excute=0;	//prevent excute process 
	Initcar_queue(&q);
	Initcar_queue(result);
	int index = 0, excute_time=0, pid=-1, time = 0;
	while (index < n || IsEmpty_Queue(&q) == 0)
	{
		while (index < n && car_arr[index].arrive_time == time)
		{
			Push_Queue(&q, &car_arr[index]);
			++index;
		}
		if (excute!=0&&excute_time == timequantum)
		{
			Push_Queue(&q, excute);
			Pop_Queue(&q);
			excute = 0;
			excute_time = 0;
		}
		if (excute == 0)
		{
			excute = Get_Queue(&q);
		}
		if (excute != 0)
		{
			Push_Queue(result, excute);
			pid=Excute_Car(excute);
			excute_time++;
		}
		else
		{
			pid = -1;
		}
		if (excute!=0&&excute->remain_time == 0)
		{
			//process terminate
			excute_time = 0;
			Pop_Queue(&q);
			excute = 0;
		}
		time++;
	}
}

// main part
pthread_t produccer, consumer0, consumer1, consumer2, consumer3, consumer4;
struct timeval start, end;
double result_T;

MAP *multiproduccer;
multiple_arg = (MA *)malloc(sizeof(MA));

gettimeofday(&start, NULL);

pthread_create(&produccer, NULL, RR,(void*)MultipleArgpro));
pthread_create(&consumer0, NULL, Pop_Queue_car,(void*)shareres);
pthread_create(&consumer1, NULL, Pop_Queue_car,(void*)shareres);
pthread_create(&consumer2, NULL, Pop_Queue_car,(void*)shareres);
pthread_create(&consumer3, NULL, Pop_Queue_car,(void*)shareres);
pthread_create(&consumer4, NULL, Pop_Queue_car,(void*)shareres);

pthread_join(&produccer, NULL);
pthread_join(&consumer0, NULL);
pthread_join(&consumer1, NULL);
pthread_join(&consumer2, NULL);
pthread_join(&consumer3, NULL);
pthread_join(&consumer4, NULL);

gettimeofday(&end, NULL);

result_T = (end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec)*0.000001);

printf("Loop time = %f\n", result_T);



