#include "../os.h"
#include "queue.h"
#include <gtk/gtk.h>
#include <math.h>

#include "memory.h"

extern OS os;
extern GtkTextBuffer *console_buffer;
extern Queue queue1;
extern Queue queue2;
extern Queue queue3;
extern Queue queue4;

Queue *queue_from_priority(const int priority) {
    switch (priority) {
        case 1:
            return &queue1;
        case 2:
            return &queue2;
        case 3:
            return &queue3;
        case 4:
            return &queue4;
        default:
            g_message("PANIC! PRIORITY NOT FOUND");
            return &queue1;
    }
}

PCB *next_process_in_queue() {
    PCB *nextPcb1 = peek(&queue1);
    PCB *nextPcb2 = peek(&queue2);
    PCB *nextPcb3 = peek(&queue3);
    PCB *nextPcb4 = peek(&queue4);

    PCB *nextPCB = NULL;

    if (nextPcb1 != NULL) {
        nextPCB = nextPcb1;
    } else if (nextPcb2 != NULL) {
        nextPCB = nextPcb2;
    } else if (nextPcb3 != NULL) {
        nextPCB = nextPcb3;
    } else if (nextPcb4 != NULL) {
        nextPCB = nextPcb4;
    }

    if (nextPCB != NULL && os.runningProcess != NULL && os.runningProcess->priority < nextPCB->priority) {
        return os.runningProcess;
    }

    return nextPCB;
}

PCB *next_process_to_run_mlfq() {
    PCB *nextPcb;
    if (os.runningProcess != NULL && os.runningProcess->quantum == pow(2, os.runningProcess->priority - 1)) {
        os.runningProcess->processState = "READY";
        if (os.runningProcess->priority != 4) os.runningProcess->priority++;
        enqueue(queue_from_priority(os.runningProcess->priority), os.runningProcess);
        os.readyQueue[os.runningProcess->processID] = os.runningProcess;
        nextPcb = next_process_in_queue();
        dequeue(queue_from_priority(nextPcb->priority));
        nextPcb->quantum = 1;

        GtkTextIter end;
        gtk_text_buffer_get_end_iter(console_buffer, &end);
        gtk_text_buffer_insert_with_tags_by_name(console_buffer, &end,
                                                 g_strdup_printf(
                                                     "SYSTEM# PREEMPTING PID %d WITH PID %d\n",
                                                     os.runningProcess->processID, nextPcb->processID), -1,
                                                 "green_fg", NULL);
        return nextPcb;
    }

    nextPcb = next_process_in_queue();
    if (nextPcb != NULL && os.runningProcess == NULL) {
        dequeue(queue_from_priority(nextPcb->priority));
        nextPcb->quantum = 1;
        return nextPcb;
    }

    if (os.runningProcess != NULL) {
        if (os.runningProcess->quantum == pow(2, os.runningProcess->priority - 1)) {
            os.runningProcess->quantum = 0;
        }
        os.runningProcess->quantum++;
    }

    return os.runningProcess;
}
