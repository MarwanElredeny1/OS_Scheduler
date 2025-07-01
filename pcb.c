#include "pcb.h"
#include <gtk/gtk.h>

char *pcbAsString(const PCB *pcb) {
    // const int id = *(int *) load_from_memory(pcb, "Process ID");
    // char *state = load_from_memory(pcb, "Process State");
    // const int priority = *(int *) load_from_memory(pcb, "Priority");
    // const int pc = *(int *) load_from_memory(pcb, "Program Counter");
    // const char *mb = load_from_memory(pcb, "Memory Boundaries");
    // return g_strdup_printf(
    //     "PID: %d\nState: %s\nPriority: %d\nPC: %d\nMemory Boundaries: %s",
    //     id,
    //     state,
    //     priority,
    //     pc,
    //     mb
    // );

    return g_strdup_printf(
        "PID: %d\nState: %s\nPriority: %d\nPC: %d\nMemory Boundaries: %d - %d",
        pcb->processID,
        pcb->processState,
        pcb->priority,
        pcb->programCounter,
        pcb->memoryBoundaries[0],
        pcb->memoryBoundaries[1]
    );
}
