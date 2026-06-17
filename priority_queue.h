#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "flight.h"

#define MAX_PQ 100

typedef struct {
    Flight *arr[MAX_PQ];
    int size;
} PriorityQueue;

void initializePriorityQueue(PriorityQueue *pq);
int isEmptyPriorityQueue(PriorityQueue *pq);
void enqueuePriority(PriorityQueue *pq, Flight *f);
Flight* dequeuePriority(PriorityQueue *pq);

#endif
