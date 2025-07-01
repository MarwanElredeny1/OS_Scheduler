#ifndef PCB_H
#define PCB_H

typedef struct PCB {
    int processID;
    char *processState;
    int priority;
    int programCounter;
    int memoryBoundaries[2];

    int arriveTick;
    int quantum;
    char *code;
} PCB;

typedef struct {
    PCB *acquiredBy;
    PCB *blockingQueue[3];
} PCBMutex;

char *pcbAsString(const PCB *pcb);

#endif
