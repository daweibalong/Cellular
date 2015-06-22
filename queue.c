#include "queue.h"

int 
is_empty(TaskQueue q)
{
	return q->size == 0;
}

int 
is_full(TaskQueue q)
{
	return q->size == q->capacity;
}

TaskQueue create_queue()
{
	TaskQueue q;
	q = (TaskQueue)malloc(sizeof(struct _task_queue));

	if (q == NULL) {
		printf("Out of space!!");
		return NULL;
	}

	q->capacity = MAX_SIZE;
	
	make_empty(q);

	return q;
}

void make_empty(TaskQueue q)
{
	q->rear = 0;
	q->front = 1;
	q->size = 0;
}

void enqueue(TaskQueue q, Task *task)
{
	if (!is_full(q)) {
		q->rear = (q->rear + 1) % q->capacity;
		q->tasks[q->rear] = task;
		q->size += 1;
	} else {
		printf("full\n");
	}
}

Task*
front(TaskQueue q) 
{
	if (!is_empty(q))
		return q->tasks[q->front];
	else
		return NULL;
}

void 
dequeue(TaskQueue q)
{
	if (!is_empty(q)) {
		q->front = (q->front + 1) % q->capacity;
		q->size--;
	} else {
		printf("empty!\n");
	}
}

Task* 
front_and_dequeue(TaskQueue q)
{
	int tmp;
	if (!is_empty(q)) {
		q->size--;
		tmp = q->front;
		q->front = (q->front + 1) % q->capacity;
		return q->tasks[tmp];
	} else {
		printf("empty!\n");
		return NULL;
	}
}

