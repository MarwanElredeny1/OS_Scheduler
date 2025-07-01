#ifndef MLFQ_H
#define MLFQ_H

#include "../pcb.h"

Queue *queue_from_priority(const int priority);

PCB *next_process_to_run_mlfq();

#endif