#pragma once

#include <stdbool.h>
#include "../pcb.h"

#define MAX_SIZE 100

typedef struct
{
    PCB *items[MAX_SIZE];
    int front;
    int rear;
} Queue;

// Function declarations
void initializeQueue(Queue *q);
bool isEmpty(Queue *q);
bool isFull(Queue *q);
void enqueue(Queue *q, PCB *value);
void dequeue(Queue *q);
PCB *peek(Queue *q);
void printQueue(Queue *q);
