#ifndef QUEUE_H
#define QUEUE_H

#include "flight.h"

#define MAX_QUEUE 100

typedef struct {
    Flight *arr[MAX_QUEUE];
    int front, rear;
} Queue;

void initializeQueue(Queue *q);
int isEmptyQueue(Queue *q);
int isFullQueue(Queue *q);
void enqueue(Queue *q, Flight *f);
Flight* dequeue(Queue *q);

#endif
