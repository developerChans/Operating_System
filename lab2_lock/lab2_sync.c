/*
*   DKU Operating System Lab
*           Lab2 (Vehicle production Problem)
*           Student id : 32191890
*           Student name : ParkChanJin(developerChans)
*
*   lab2_sync.c :
*       - lab2 main file.
*       - must contains Vehicle production Problem function's declations.
*
*/

#include <aio.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <asm/unistd.h>

#include "lab2_sync_types.h"


void Init_car(car* target, int arrive_time, int service_time, int pid)
{
	target->arrive_time = arrive_time;
	target->service_time = target->remain_time = service_time;
	target->pid = pid;
	target->priority = 0;
}

int Excute_Car(car* pro)
{
	if (pro == 0)
	{
		return -1;
	}
	pro->remain_time -= 1;
	return pro->pid;
}

void Sort(car* car_arr, int n)
{
	for (int i = 0; i < n - 1; ++i)
	{
		car temp;
		int index = i;
		for (int j = i + 1; j < n; ++j)
		{
			if (car_arr[index].arrive_time > car_arr[j].arrive_time)
			{
				index = j;
			}
		}
		temp = car_arr[i];
		car_arr[i] = car_arr[index];
		car_arr[index] = temp;
	}
}

node* CreatNode(car *p) {
	node* ret =(node*)malloc(sizeof(node));
	ret->process_ele = *p;
	ret->next = NULL;
	return ret;
}

void FreeNode(node*target) {
	free(target);
}

void Initcar_queue (cq* q) {
	q->front = q->rear = NULL;
	q->balance = 0;
}

void Push_Queue(cq *q, car *p)
{
	if(q->front ==0)
	{
		q->front = q->rear = CreatNode(p);
	}
	else
	{
		q->rear->next = CreatNode(p);
		q->rear = q->rear->next;
	}	
}
void Pop_Queue(cq *q) {

	if(IsEmpty_Queue(q) == 0) {
		node *temp = q->front;
		q->front = q->front->next;
		FreeNode(temp);
	}
}

pthread_cond_t empty, fill;
pthread_mutex_t mutex;
pthread_mutex_t mutex1, mutex2;

void Pop_Queue_CON(cq *q,int n, int carnum) {

		if(IsEmpty_Queue(q) == 0) {
			node *temp = q->front;
			q->front = q->front->next;
			FreeNode(temp);
		}
		printf("pop queue before balance : %d\n",q->balance);
		q->balance--;
		n--;
		printf("pop queue after balance : %d\n",q->balance);
	}




void *Pop_Queue_car(void * conmu1) {
	CONMA *con_arg = (CONMA *)conmu1;
	for(int i = 0; i<con_arg->looplen; i++) {
		
		pthread_mutex_lock(&mutex);
		while (con_arg->sharequeue->balance == 0)
			pthread_cond_wait(&fill, &mutex);
		pthread_mutex_lock(&mutex);
		if(con_arg->sharequeue->front->process_ele.pid == con_arg->whatcar) 
			Pop_Queue_CON(con_arg->sharequeue,con_arg->total,con_arg->whatcar);
		pthread_mutex_unlock(&mutex);
		printf("pop_queue finish %d\n",i);
		
		pthread_cond_signal(&empty);
		pthread_mutex_unlock(&mutex);
	}
}

/*

void *Pop_Queue_car(void * conmu1) {
	CONMA *con_arg = (CONMA *)conmu1;
	for(int i = 0; i<con_arg->looplen; i++) {
		//pthread_mutex_lock(&mutex);
		if (con_arg->sharequeue->front->process_ele.pid == con_arg->whatcar) {
			if(IsEmpty_Queue(con_arg->sharequeue) == 0) {
				node *temp = con_arg->sharequeue->front;
				con_arg->sharequeue->front = con_arg->sharequeue->front->next;
				FreeNode(temp);
			}
			con_arg->sharequeue->balance -= 1;
			con_arg->total--;
		}	
		pthread_mutex_unlock(&mutex);
	}
}
*/
car* Get_Queue(cq *q) {
	return &(q->front->process_ele);
}

int IsEmpty_Queue(cq *q) {
	return q->front ==NULL;
}




/////////////////////////////////////////////



void *RR(void *multproduccer)
{
	MAP *pro_arg = (MAP *)multproduccer;
	cq q;
	car* excute=0;	//prevent excute process 
	Initcar_queue(&q);
	
	int index = 0, excute_time=0, pid=-1, time = 0;
	
	while (index < pro_arg->k || IsEmpty_Queue(&q) == 0)
	{
		while (index < pro_arg->k && pro_arg->car_arr[index].arrive_time == time)
		{
			Push_Queue(&q, &pro_arg->car_arr[index]);
			++index;
		}		
		if (excute!=0&&excute_time == pro_arg->timequantums)
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
		
			pthread_mutex_lock(&mutex1);
			while(pro_arg->result->balance == MAX)
				pthread_cond_wait(&empty,&mutex1);

			Push_Queue(pro_arg->result,excute);
			pro_arg->result->balance++;
			printf("push queue %d\n", pro_arg->result->balance);
			
			pthread_cond_signal(&fill);
			pthread_mutex_unlock(&mutex1);

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

int main() {
	car* arr = 0;
	int n, timequantum;
	int sumloop = 0;
	cq *shareres=0;
	pthread_t produccer, consumer0, consumer1, consumer2, consumer3, consumer4;
	
	printf("Input Process count: ");
	scanf("%d", &n);
	
	arr = (car*)malloc(sizeof(car)*n);
	shareres =(cq*)malloc(sizeof(cq) * 11);
	CONMA *consumermulti[5];
	Initcar_queue(shareres);
	for (int i = 0; i < 5; i ++) {
		consumermulti[i] = (CONMA *)malloc(sizeof(CONMA));
	}
	for (int i = 0; i < n; ++i)
	{
		int arrive_time, service_time;
		consumermulti[i]->whatcar = i;
		printf("Input Process %d \'s arrive time and service time: ",i);
		scanf("%d %d", &arrive_time, &service_time);
		consumermulti[i]->looplen = service_time;
		consumermulti[i]->sharequeue = shareres;
		sumloop += service_time;
		Init_car(arr + i, arrive_time, service_time, i);
	}
	for (int i = 0; i < 5; i ++) {
		consumermulti[i]->total = sumloop;
	}
	Sort(arr, n);
	printf("Input Timequantum of RoundRobin: ");
	scanf("%d", &timequantum);
	for(int i = 0; i<5; i++) 
		printf("Car %d total made : %d\n",i,consumermulti[i]->looplen);
	
	struct timeval start, end;
	double result_T;
	
	MAP *multiproduccer;
	multiproduccer = (MAP *)malloc(sizeof(MAP));
	multiproduccer->k = n;
	multiproduccer->car_arr = arr;
	multiproduccer->timequantums = timequantum;
	multiproduccer->result = shareres;

	gettimeofday(&start, NULL);
	pthread_create(&produccer, NULL, RR,(void*)multiproduccer);
	
	pthread_create(&consumer0, NULL, Pop_Queue_car,(void*)consumermulti[0]);
	pthread_create(&consumer1, NULL, Pop_Queue_car,(void*)consumermulti[1]);
	pthread_create(&consumer2, NULL, Pop_Queue_car,(void*)consumermulti[2]);
	pthread_create(&consumer3, NULL, Pop_Queue_car,(void*)consumermulti[3]);
	pthread_create(&consumer4, NULL, Pop_Queue_car,(void*)consumermulti[4]);


	pthread_join(produccer, NULL);
	pthread_join(consumer0, NULL);
	pthread_join(consumer1, NULL);
	pthread_join(consumer2, NULL);
	pthread_join(consumer3, NULL);
	pthread_join(consumer4, NULL);

	gettimeofday(&end, NULL);
	printf("balance : %d\n",shareres->balance);
	result_T = (end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec)*0.000001);

	printf("Loop time = %f\n", result_T);	
	printf("total car : %d\n",consumermulti[0]->total);	
}


	
	
	
	
