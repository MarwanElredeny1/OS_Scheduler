#ifndef OS_H
#define OS_H

#include "pcb.h"

void run_process(int pid);

enum SchedulerType {
    FCFS,
    RoundRobin,
    MLFQ,
};

typedef enum {
    FileAccess = 0,
    InputAccess,
    OutputAccess,
} BlockType;

typedef struct {
    int tick;
    int quantum;
    enum SchedulerType selectedScheduler;

    int numberOfProcesses;
    PCB *processes[3];

    PCB *readyQueue[3];
    PCB *blockingQueue[3];
    PCB *runningProcess;

    PCBMutex rwMutex;
    PCBMutex inputMutex;
    PCBMutex outputMutex;
} OS;

void tick();

#endif
