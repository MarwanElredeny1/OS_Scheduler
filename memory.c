#include "memory.h"
#include "ui/memory_viewer.h"
#include "pcb.h"
#include <gtk/gtk.h>

extern Word words[60];

extern GtkWidget *memoryViewerBox;
extern GtkTextBuffer *console_buffer;

char *load_str_word_from_memory(const PCB *pcb, const char *wordName) {
    for (int i = 20 * pcb->processID; i < 20 * pcb->processID + VARIABLES_OFFSET + 5; i++) {
        if (words[i].name != NULL && strcmp(words[i].name, wordName) == 0) {
            return words[i].valueStr != NULL ? words[i].valueStr : g_strdup_printf("%d", words[i].valueInt);
        }
    }
    return NULL;
}

int load_int_word_from_memory(const PCB *pcb, const char *wordName) {
    for (int i = 20 * pcb->processID; i < 20 * pcb->processID + VARIABLES_OFFSET + 5; i++) {
        if (words[i].name != NULL && strcmp(words[i].name, wordName) == 0) {
            return words[i].valueInt;
        }
    }
    return 0;
}

void update_int_word_in_memory(const PCB *pcb, const char *wordName, const int value) {
    for (int i = 20 * pcb->processID; i < 20 * pcb->processID + VARIABLES_OFFSET + 5; i++) {
        if (words[i].name != NULL && strcmp(words[i].name, wordName) == 0) {
            words[i].valueInt = value;
            return;
        }
    }
}

void update_str_word_in_memory(const PCB *pcb, const char *wordName, const char *value) {
    for (int i = 20 * pcb->processID; i < 20 * pcb->processID + VARIABLES_OFFSET + 5; i++) {
        if (words[i].name != NULL && strcmp(words[i].name, wordName) == 0) {
            words[i].valueStr = g_strdup(value);
            return;
        }
    }
}

void updatePCBInMemory(PCB *pcb) {
    int wordIndex = pcb->processID * 20;
    words[wordIndex].name = "Process ID";
    words[wordIndex].valueInt = pcb->processID;
    words[wordIndex++].type = TYPE_INT;

    words[wordIndex].name = "Process State";
    words[wordIndex].valueStr = pcb->processState;
    words[wordIndex++].type = TYPE_STRING;

    words[wordIndex].name = "Priority";
    words[wordIndex].valueInt = pcb->priority;
    words[wordIndex++].type = TYPE_INT;

    words[wordIndex].name = "Program Counter";
    words[wordIndex].valueInt = pcb->programCounter;
    words[wordIndex++].type = TYPE_INT;

    words[wordIndex].name = "Memory Boundaries";
    words[wordIndex].valueStr = g_strdup_printf("%d - %d", pcb->memoryBoundaries[0], pcb->memoryBoundaries[1]);
    words[wordIndex++].type = TYPE_STRING;

    refresh_table_layout();
}
