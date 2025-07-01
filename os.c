#include "os.h"

#include <stdbool.h>
#include <string.h>
#include <tgmath.h>
#include <gtk/gtk.h>

#include "memory.h"
#include "pcb.h"
#include "schedulers/queue.h"
#include "schedulers/fcfs.h"
#include "schedulers/round_robin.h"
#include "schedulers/mlfq.h"

extern GtkWidget *input_entry;
extern GtkTextBuffer *console_buffer;

extern OS os;
extern Word words[60];
extern PCB *pcb_waiting_for_input;
extern bool isTicking;
extern char code[8192];

char *variableName;

extern Queue queue1;
extern Queue queue2;
extern Queue queue3;
extern Queue queue4;

void run_process(const int pid) {
    PCB *pcb = os.processes[pid];
    os.readyQueue[pid] = NULL;
    os.runningProcess = pcb;

    if (pcb->processState == "BLOCKED") {
        os.blockingQueue[pid] = NULL;
        os.rwMutex.blockingQueue[pid] = NULL;
        os.inputMutex.blockingQueue[pid] = NULL;
        os.outputMutex.blockingQueue[pid] = NULL;
    }

    pcb->processState = "RUNNING";

    updatePCBInMemory(pcb);
}

void blockRunningProcess() {
    PCB *pcb = os.runningProcess;
    const int pid = pcb->processID;
    os.blockingQueue[pid] = pcb;
    os.runningProcess = NULL;
    pcb->processState = "BLOCKED";
    updatePCBInMemory(pcb);
}

void lockRunningProcess(const BlockType blockType) {
    PCB *pcb = os.runningProcess;
    const int pid = pcb->processID;
    blockRunningProcess();

    GtkTextIter end;
    gtk_text_buffer_get_end_iter(console_buffer, &end);
    switch (blockType) {
        case FileAccess:
            os.rwMutex.blockingQueue[pid] = pcb;
            gtk_text_buffer_insert_with_tags_by_name(console_buffer, &end,
                                                     g_strdup_printf(
                                                         "SYSTEM# FILE ACCESS LOCK ALREADY ACQUIRED BY PID %d | BLOCKING PID %d\n",
                                                         os.rwMutex.acquiredBy->processID, pcb->processID), -1,
                                                     "green_fg", NULL);
            break;
        case InputAccess:
            os.inputMutex.blockingQueue[pid] = pcb;
            gtk_text_buffer_insert_with_tags_by_name(console_buffer, &end,
                                                     g_strdup_printf(
                                                         "SYSTEM# INPUT ACCESS LOCK ALREADY ACQUIRED BY PID %d | BLOCKING PID %d\n",
                                                         os.inputMutex.acquiredBy->processID, pcb->processID), -1,
                                                     "green_fg", NULL);
            break;
        case OutputAccess:
            os.outputMutex.blockingQueue[pid] = pcb;
            gtk_text_buffer_insert_with_tags_by_name(console_buffer, &end,
                                                     g_strdup_printf(
                                                         "SYSTEM# OUTPUT ACCESS LOCK ALREADY ACQUIRED BY PID %d | BLOCKING PID %d\n",
                                                         os.outputMutex.acquiredBy->processID, pcb->processID), -1,
                                                     "green_fg", NULL);
            break;
        default:



    }
}

PCB *unblock_pcb(PCBMutex *mutex) {
    mutex->acquiredBy = NULL;

    PCB *pcbToUnblock = NULL;
    for (int j = 0; j < 3; j++) {
        PCB *blockedPcb = mutex->blockingQueue[j];
        if (
            blockedPcb != NULL && (pcbToUnblock == NULL || blockedPcb->priority < pcbToUnblock->priority || blockedPcb->arriveTick <= pcbToUnblock->arriveTick)
        ) {
            pcbToUnblock = blockedPcb;
        }
    }
    if (pcbToUnblock != NULL) {
        mutex->blockingQueue[pcbToUnblock->processID] = NULL;
        os.blockingQueue[pcbToUnblock->processID] = NULL;
        mutex->acquiredBy = pcbToUnblock;
        os.readyQueue[pcbToUnblock->processID] = pcbToUnblock;
        if (os.selectedScheduler == MLFQ) {
            if (pcbToUnblock->quantum == pow(2, pcbToUnblock->priority - 1) && pcbToUnblock->priority != 4) pcbToUnblock->priority++;
            enqueue(queue_from_priority(pcbToUnblock->priority), pcbToUnblock);
        } else enqueue(&queue1, pcbToUnblock);
        pcbToUnblock->processState = "READY";
        updatePCBInMemory(pcbToUnblock);

        return pcbToUnblock;
    }

    return NULL;
}

void executeRunningProcessNextInstruction() {
    PCB *pcb = os.runningProcess;
    int pc = load_int_word_from_memory(pcb, "Program Counter");
    const char *instruction = words[pc].valueStr;
    update_int_word_in_memory(pcb, "Program Counter", pc + 1);
    pcb->programCounter = pc + 1;
    GtkTextIter iter;
    gtk_text_buffer_get_end_iter(console_buffer, &iter);
    gtk_text_buffer_insert_with_tags_by_name(console_buffer, &iter,
                                             g_strdup_printf("OS [T%d]# Executed instruction of PID %d: %s\n", os.tick, pcb->processID, instruction),
                                             -1, "blue_fg", NULL);
    char buffer[100];
    strncpy(buffer, instruction, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    char *instrArgs[10];

    int i = 0;
    char *token = strtok(buffer, " ");
    while (token != NULL && i < 10) {
        instrArgs[i++] = token;
        token = strtok(NULL, " ");
    }

    const char *a1 = instrArgs[0];
    const char *a2 = instrArgs[1];
    const char *a3 = instrArgs[2];
    const char *a4 = instrArgs[3];

    if (strcmp(a1, "semWait") == 0) {
        if (strcmp(a2, "userInput") == 0) {
            if (os.inputMutex.acquiredBy == NULL) {
                os.inputMutex.acquiredBy = pcb;

                GtkTextIter end;
                gtk_text_buffer_get_end_iter(console_buffer, &end);
                gtk_text_buffer_insert_with_tags_by_name(console_buffer, &end,
                                                         g_strdup_printf(
                                                             "SYSTEM# INPUT LOCK ACQUIRED BY PID %d\n", pcb->processID),
                                                         -1, "green_fg", NULL);
            } else lockRunningProcess(InputAccess);
        } else if (strcmp(a2, "userOutput") == 0) {
            if (os.outputMutex.acquiredBy == NULL) {
                os.outputMutex.acquiredBy = pcb;

                GtkTextIter end;
                gtk_text_buffer_get_end_iter(console_buffer, &end);
                gtk_text_buffer_insert_with_tags_by_name(console_buffer, &end,
                                                         g_strdup_printf(
                                                             "SYSTEM# OUTPUT LOCK ACQUIRED BY PID %d\n",
                                                             pcb->processID), -1, "green_fg", NULL);
            } else lockRunningProcess(OutputAccess);
        } else {
            if (os.rwMutex.acquiredBy == NULL) {
                os.rwMutex.acquiredBy = pcb;

                GtkTextIter end;
                gtk_text_buffer_get_end_iter(console_buffer, &end);
                gtk_text_buffer_insert_with_tags_by_name(console_buffer, &end,
                                                         g_strdup_printf(
                                                             "SYSTEM# FILE ACCESS LOCK ACQUIRED BY PID %d\n",
                                                             pcb->processID), -1, "green_fg", NULL);
            } else lockRunningProcess(FileAccess);
        }
    } else if (strcmp(a1, "semSignal") == 0) {
        if (strcmp(a2, "userInput") == 0) {
            if (os.inputMutex.acquiredBy == pcb) {
                PCB *unblockedPCB = unblock_pcb(&os.inputMutex);

                GtkTextIter end;
                gtk_text_buffer_get_end_iter(console_buffer, &end);
                gtk_text_buffer_insert_with_tags_by_name(console_buffer, &end,
                                                         g_strdup_printf(
                                                             "SYSTEM# INPUT ACCESS LOCK RELEASED BY PID %d%s\n",
                                                             pcb->processID,
                                                             unblockedPCB != NULL ? g_strdup_printf(" | Unblocking PID %d which now acquired this mutex", unblockedPCB->processID) : ""
                                                             ), -1, "red_fg", NULL);
            }
        } else if (strcmp(a2, "userOutput") == 0) {
            if (os.outputMutex.acquiredBy == pcb) {
                PCB *unblockedPCB = unblock_pcb(&os.outputMutex);

                GtkTextIter end;
                gtk_text_buffer_get_end_iter(console_buffer, &end);
                gtk_text_buffer_insert_with_tags_by_name(console_buffer, &end,
                                                         g_strdup_printf(
                                                             "SYSTEM# OUTPUT ACCESS LOCK RELEASED BY PID %d%s\n",
                                                             pcb->processID,
                                                             unblockedPCB != NULL ? g_strdup_printf(" | Unblocking PID %d which now acquired this mutex", unblockedPCB->processID) : ""
                                                             ), -1, "red_fg", NULL);
            }
        } else {
            if (os.rwMutex.acquiredBy == pcb) {
                PCB *unblockedPCB = unblock_pcb(&os.rwMutex);

                GtkTextIter end;
                gtk_text_buffer_get_end_iter(console_buffer, &end);
                gtk_text_buffer_insert_with_tags_by_name(console_buffer, &end,
                                                         g_strdup_printf(
                                                             "SYSTEM# FILE ACCESS LOCK RELEASED BY PID %d%s\n",
                                                             pcb->processID,
                                                             unblockedPCB != NULL ? g_strdup_printf(" | Unblocking PID %d which now acquired this mutex", unblockedPCB->processID) : ""
                                                             ), -1, "red_fg", NULL);
            }
        }
    } else if (strcmp(a1, "assign") == 0) {
        if (strcmp(a3, "input") == 0) {
            GtkTextIter end;
            gtk_text_buffer_get_end_iter(console_buffer, &end);
            gtk_text_buffer_insert(console_buffer, &end, "Please enter a value: ", -1);

            pcb_waiting_for_input = pcb;
            variableName = g_strdup(a2);
            isTicking = false;
        } else if (strcmp(a3, "readFile") == 0) {
            char *a = load_str_word_from_memory(pcb, a4);

            FILE *file = fopen(a, "r");

            if (file == NULL) {
                printf("Error opening file!\n");
                return;
            }

            int wordIndex;

            for (int j = 20 * pcb->processID + VARIABLES_OFFSET; j < 20 * pcb->processID + VARIABLES_OFFSET + 5; j++) {
                if (words[j].name == NULL) {
                    wordIndex = j;
                    words[j].name = g_strdup(a2);
                    words[j].type = TYPE_STRING;
                    break;
                }
            }

            char line[100];
            while (fgets(line, sizeof(line), file)) {
                char *wordValue = words[wordIndex].valueStr == NULL ? "" : words[wordIndex].valueStr;
                char *text = g_strconcat(wordValue, line, NULL);
                words[wordIndex].valueStr = text;
            }

            fclose(file);
        }
    } else if (strcmp(a1, "printFromTo") == 0) {
        int a = load_int_word_from_memory(pcb, a2);
        int b = load_int_word_from_memory(pcb, a3);

        if (a != -1 && b != -1) {
            const int min = a < b ? a : b;
            const int max = a >= b ? a : b;

            GtkTextIter end;
            gtk_text_buffer_get_end_iter(console_buffer, &end);
            for (int j = min; j <= max; j++) {
                gtk_text_buffer_insert(console_buffer, &end, g_strdup_printf("%d\n", j), -1);
            }
        }
    } else if (strcmp(a1, "writeFile") == 0) {
        char *a = load_str_word_from_memory(pcb, a2);
        char *b = load_str_word_from_memory(pcb, a3);

        FILE *file = fopen(a, "w");

        if (file == NULL) {
            printf("Error opening file!\n");
            return;
        }

        fprintf(file, b);
        fclose(file);
    } else if (strcmp(a1, "print") == 0) {
        for (int j = 20 * pcb->processID + VARIABLES_OFFSET; j < 20 * pcb->processID + VARIABLES_OFFSET + 5; j++) {
            const char *name = words[j].name;
            const char *value = words[j].valueStr;
            if (strcmp(name, a2) == 0) {
                GtkTextIter end;
                gtk_text_buffer_get_end_iter(console_buffer, &end);
                gtk_text_buffer_insert(console_buffer, &end, g_strdup_printf("%s\n", value), -1);
                break;
            }
        }
    }

    int newPC = load_int_word_from_memory(pcb, "Program Counter");
    if (words[newPC].valueStr == NULL) {
        pcb->processState = "TERMINATED";
        os.runningProcess = NULL;

        updatePCBInMemory(pcb);
    }
}

void remove_carriage_returns(char *str) {
    const char *src = str;
    char *dst = str;
    while (*src) {
        if (*src != '\r') {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';
}

void tick() {
    for (int i = 0; i < 3; i++) {
        PCB *pcb = os.processes[i];
        if (pcb != NULL && pcb->arriveTick == os.tick) {
            const int pid = pcb->processID;
            const int initialProcessCodeWord = (20 * pid) + CODE_OFFSET;

            int wordIndex = (20 * pid) + CODE_OFFSET;
            char *line = strtok(pcb->code, "\n");
            while (line != NULL) {
                remove_carriage_returns(line);
                words[wordIndex].name = g_strdup_printf("CL %d", wordIndex - initialProcessCodeWord);
                words[wordIndex].valueStr = line;
                words[wordIndex].type = TYPE_STRING;
                line = strtok(NULL, "\n");
                wordIndex++;
            }

            pcb->priority = 1;
            updatePCBInMemory(pcb);

            os.readyQueue[pid] = pcb;
            enqueue(&queue1, pcb);
        }
    }

    const PCB *pcb = NULL;

    switch (os.selectedScheduler) {
        case FCFS:
            if (os.runningProcess == NULL) pcb = next_process_to_run_fcfs();
            break;
        case RoundRobin:
            pcb = next_process_to_run_round_robin();
            break;
        case MLFQ:
            pcb = next_process_to_run_mlfq();
            break;
        default:
    }

    if (pcb != NULL)
        run_process(pcb->processID);

    if (os.runningProcess != NULL) {
        executeRunningProcessNextInstruction();
    }
}
