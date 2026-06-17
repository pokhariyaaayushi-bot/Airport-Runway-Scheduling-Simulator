#include "queue.h"
#include <stdio.h>

void initializeQueue(Queue *q) {
    q->front = q->rear = -1;
}

int isEmptyQueue(Queue *q) {
    return q->front == -1;
}

int isFullQueue(Queue *q) {
    return (q->rear + 1) % MAX_QUEUE == q->front;
}

void enqueue(Queue *q, Flight *f) {
    if (isFullQueue(q)) {
        printf("Queue Full! Cannot enqueue.\n");
        return;
    }
    if (isEmptyQueue(q)) q->front = 0;
    q->rear = (q->rear + 1) % MAX_QUEUE;
    q->arr[q->rear] = f;
}

Flight* dequeue(Queue *q) {
    if (isEmptyQueue(q)) return NULL;
    Flight *temp = q->arr[q->front];
    if (q->front == q->rear)
        q->front = q->rear = -1;
    else
        q->front = (q->front + 1) % MAX_QUEUE;
    return temp;
}
