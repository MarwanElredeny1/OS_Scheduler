#include <gtk/gtk.h>
#include "../pcb.h"
#include "../os.h"

extern OS os;
GtkWidget *mutexGridBox;

void setMutexProcessesList(GtkWidget *parent, const BlockType type, const PCBMutex pcbMutex, char *title) {
    GtkWidget *mutexBox = NULL;
    switch (type) {
        case FileAccess:
            mutexBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
            break;
        case InputAccess:
            mutexBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
            break;
        case OutputAccess:
            mutexBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
            break;
        default:
    }

    GtkWidget *mutexLabel = gtk_label_new(g_strdup_printf(
            "%s (%s):\n\nHolding Process:\n%s\n\n\nBocked Processes:\n",
            title,
            pcbMutex.acquiredBy == NULL ? "AVAILABLE" : "IN USE",
            pcbMutex.acquiredBy == NULL ? "NONE" : pcbAsString(pcbMutex.acquiredBy))
    );
    GtkWidget *processes_scroll_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(processes_scroll_window, 1, 200);
    GtkWidget *processesList = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(processes_scroll_window), processesList);
    for (int i = 0; i < os.numberOfProcesses; i++) {
        PCB *pcb = pcbMutex.blockingQueue[i];
        if (pcb == NULL) continue;

        GtkWidget *label = gtk_label_new(pcbAsString(pcb));

        gtk_widget_set_halign(label, GTK_ALIGN_START);
        gtk_list_box_insert(GTK_LIST_BOX(processesList), label, -1);
    }
    gtk_box_pack_start(GTK_BOX(mutexBox), mutexLabel, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(mutexBox), processes_scroll_window, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(parent), mutexBox, TRUE, TRUE, 0);
}

void refresh_mutex_grid() {
    GList *children = gtk_container_get_children(GTK_CONTAINER(mutexGridBox));
    for (GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    setMutexProcessesList(mutexGridBox, FileAccess, os.rwMutex, "FILE ACCESS");
    setMutexProcessesList(mutexGridBox, InputAccess, os.inputMutex, "INPUT ACCESS");
    setMutexProcessesList(mutexGridBox, OutputAccess, os.outputMutex, "OUTPUT ACCESS");

    gtk_widget_show_all(mutexGridBox);
}

GtkWidget *create_resource_management_tab() {
    mutexGridBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_valign(mutexGridBox, GTK_ALIGN_START);

    refresh_mutex_grid();

    return mutexGridBox;
}
