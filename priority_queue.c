#include "priority_queue.h"
#include <stdio.h>

int getEmergencyPriority(EmergencyType e) {
    switch (e) {
        case ENGINE_FAILURE: return 3;
        case MEDICAL: return 2;
        case FUEL_LOW: return 1;
        default: return 0;
    }
}

void initializePriorityQueue(PriorityQueue *pq) {
    pq->size = 0;
}

int isEmptyPriorityQueue(PriorityQueue *pq) {
    return pq->size == 0;
}

void enqueuePriority(PriorityQueue *pq, Flight *f) {
    if (pq->size == MAX_PQ) {
        printf("Priority queue full!\n");
        return;
    }
    pq->arr[pq->size++] = f;

    int i = pq->size - 1;
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (getEmergencyPriority(pq->arr[i]->emergency) > 
            getEmergencyPriority(pq->arr[parent]->emergency)) {
            Flight *temp = pq->arr[i];
            pq->arr[i] = pq->arr[parent];
            pq->arr[parent] = temp;
            i = parent;
        } else break;
    }
}

Flight* dequeuePriority(PriorityQueue *pq) {
    if (isEmptyPriorityQueue(pq)) return NULL;
    Flight *top = pq->arr[0];
    pq->arr[0] = pq->arr[--pq->size];

    int i = 0;
    while (1) {
        int l = 2 * i + 1, r = 2 * i + 2, largest = i;

        if (l < pq->size && 
            getEmergencyPriority(pq->arr[l]->emergency) > 
            getEmergencyPriority(pq->arr[largest]->emergency))
            largest = l;

        if (r < pq->size && 
            getEmergencyPriority(pq->arr[r]->emergency) > 
            getEmergencyPriority(pq->arr[largest]->emergency))
            largest = r;

        if (largest != i) {
            Flight *temp = pq->arr[i];
            pq->arr[i] = pq->arr[largest];
            pq->arr[largest] = temp;
            i = largest;
        } else break;
    }

    return top;
}
