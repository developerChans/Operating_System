/*
*   DKU Operating System Lab
*           Lab2 (Vehicle Production Problem)
*           Student id : 
*           Student name : 
*
*   lab1_sync_types.h :
*       - lab2 header file.
*       - must contains Vehicle Production Problem's declations.
*
*/

#ifndef _LAB2_HEADER_H
#define _LAB2_HEADER_H

#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <asm/unistd.h>
#include <stdlib.h>
#define MAX_SIZE 100

/*
 * You need to Declare functions in  here
 */

typedef struct _process
{
	int arrive_time;	//created time of process
	int service_time;	//excute time of process
	int remain_time;	//remain time to terminated
	int priority;		//priority of process
	int pid;			
}car;

int Excute_process(car* pro);
void Init_car(car* target, int arrive_time, int service_time, int pid);
void Sort(car* car_arr, int n);

typedef struct _Node 
{
	car process_ele;
	struct _Node *next;
}node;

typedef struct _carqueue 
{
	int balance;
	node *front, *rear;
}cq;


node* CreatNode(car *p);
void FreeNode(node *target);
void InitcarQueue(cq *q);
void Push_Queue(cq *q, car* p);
void Push_producerQueue(cq *q, car *p);
void Pop_Queue(cq *q);
int IsEmpty_Queue(cq *q);
car* Get_Queue(cq* q);
void *Pop_Queue_car(void * conmu1);
void Pop_Queue_CON(cq *q,int n,int carnum);
void *RR(void *multproduccer);

typedef struct MultipleArgpro {
	cq *result;
	car *car_arr;
	int k;
	int timequantums;
}MAP;

int MAX = 10;

typedef struct _Consumermult {
	cq *sharequeue;
	int whatcar;
	int looplen;
	int total;
}CONMA;
#endif /* LAB2_HEADER_H*/


