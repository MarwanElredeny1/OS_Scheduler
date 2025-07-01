#include <stddef.h>

#include "../os.h"

extern OS os;

PCB *next_process_to_run_fcfs() {
    PCB *runningProcess = NULL;

    for (int i = 0; i < 3; i++) {
        if (os.readyQueue[i] == NULL)
            continue;
        const int anotherTick = os.readyQueue[i]->arriveTick;

        if (runningProcess == NULL || runningProcess->arriveTick > anotherTick) {
            runningProcess = os.readyQueue[i];
        }
    }

    if (runningProcess == NULL || runningProcess->arriveTick > os.tick) {
        return NULL;
    }

    return runningProcess;
}
