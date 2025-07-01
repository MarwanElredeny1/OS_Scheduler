#include "../os.h"
#include "queue.h"
#include <gtk/gtk.h>

#include "memory.h"

extern OS os;
extern GtkTextBuffer *console_buffer;
extern Queue queue1;

PCB *next_process_to_run_round_robin() {
    printQueue(&queue1);

    PCB *nextPcb = peek(&queue1);
    if (nextPcb != NULL) {
        if (os.runningProcess != NULL && os.runningProcess->quantum == os.quantum) {
            GtkTextIter end;
            gtk_text_buffer_get_end_iter(console_buffer, &end);
            gtk_text_buffer_insert_with_tags_by_name(console_buffer, &end,
                                         g_strdup_printf(
                                             "SYSTEM# PREEMPTING PID %d WITH PID %d\n",
                                             os.runningProcess->processID, nextPcb->processID), -1,
                                         "green_fg", NULL);

            os.runningProcess->processState = "READY";
            enqueue(&queue1, os.runningProcess);
            os.readyQueue[os.runningProcess->processID] = os.runningProcess;
            dequeue(&queue1);
            nextPcb->quantum = 1;
            return nextPcb;
        }

        if (os.runningProcess == NULL) {
            dequeue(&queue1);
            nextPcb->quantum = 1;
            return nextPcb;
        }
    }

    if (os.runningProcess != NULL) {
        os.runningProcess->quantum++;
    }

    return os.runningProcess;
}
