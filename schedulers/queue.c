#include <stdbool.h>
#include <stdio.h>
#include "../pcb.h"
#include "queue.h"

void initializeQueue(Queue *q)
{
    q->front = -1;
    q->rear = 0;
}

bool isEmpty(Queue *q) { return (q->front == q->rear - 1); }

bool isFull(Queue *q) { return (q->rear == MAX_SIZE); }

void enqueue(Queue *q, PCB *value)
{
    if (isFull(q))
    {
        printf("Queue is full\n");
        return;
    }
    q->items[q->rear] = value;
    q->rear++;
}

void dequeue(Queue *q)
{
    if (isEmpty(q))
    {
        return;
    }
    q->front++;
}

PCB *peek(Queue *q)
{
    if (isEmpty(q))
    {
        return NULL;
    }
    return q->items[q->front + 1];
}

void printQueue(Queue *q)
{
    if (isEmpty(q))
    {
        printf("Queue is empty\n");
        return;
    }

    printf("Current Queue: ");
    for (int i = q->front + 1; i < q->rear; i++)
    {
        printf("%d ", q->items[i]->processID);
    }
    printf("\n");
}