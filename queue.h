#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdio.h>
#include <stdlib.h>

#include "box.h"

#define MAX_SIZE 10000

typedef struct {
	Box        *box;
	const char *func_name;
} Task;

struct _task_queue {
	int rear;

	int front;
	
	int size;         //队列中元素的个数
	
	int capacity;
	
	Task *tasks[MAX_SIZE];
};

typedef struct _task_queue* TaskQueue;

int
is_empty(TaskQueue q);

int
is_full(TaskQueue q);

TaskQueue
create_queue();

void make_empty(TaskQueue q);

void enqueue(TaskQueue q, Task *task);

Task*
front(TaskQueue q);

void 
dequeue(TaskQueue q);

Task *
front_and_dequeue(TaskQueue q);

#endif
