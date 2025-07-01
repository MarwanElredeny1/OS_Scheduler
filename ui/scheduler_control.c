#include <gtk/gtk.h>

#include "dashboard.h"
#include "memory_viewer.h"
#include "resource_management.h"
#include "visualization_execution.h"
#include "../memory.h"
#include "../os.h"
#include "../schedulers/queue.h"

extern OS os;
extern Word words[60];
extern GtkTextBuffer *console_buffer;

GtkWidget *quantumLabel;
GtkWidget *quantumEntry;

GtkWidget *startSimButton;
GtkWidget *stopSimButton;
GtkWidget *resetSimButton;

extern GtkWidget *executeNextButton;
extern GtkWidget *executeAutoButton;
extern GtkWidget* noProcessesFound;

extern Queue queue1;
extern Queue queue2;
extern Queue queue3;
extern Queue queue4;

void on_quantum_changed(GtkEntry *entry, gpointer data) {
    const gchar *text = gtk_entry_get_text(entry);
    const int newQuantum = atoi(text);

    if (newQuantum != os.quantum) {
        os.quantum = newQuantum;
    }
}

void addQuantumInput(GtkWidget *parentBox) {
    quantumLabel = gtk_label_new("Edit quantum (in ticks)");
    quantumEntry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(quantumEntry), g_strdup_printf("%d", os.quantum));

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(box), quantumLabel, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), quantumEntry, TRUE, TRUE, 0);
    g_signal_connect(quantumEntry, "changed", G_CALLBACK(on_quantum_changed), NULL);

    gtk_box_pack_start(GTK_BOX(parentBox), box, FALSE, FALSE, 0);

    refresh_dashboard_tab();
}

void on_combo_changed(GtkComboBoxText *widget, gpointer data) {
    const char *selectedText = gtk_combo_box_text_get_active_text(widget);
    if (strcmp(selectedText, "FCFS") == 0) {
        os.selectedScheduler = FCFS;
    } else if (strcmp(selectedText, "Round Robin") == 0) {
        os.selectedScheduler = RoundRobin;
    } else if (strcmp(selectedText, "MultiLevel Feedback Queue") == 0) {
        os.selectedScheduler = MLFQ;
    }

    refresh_dashboard_tab();
}

void on_start_simulation_clicked(GtkButton *button, gpointer data) {
    gtk_widget_set_sensitive(startSimButton, FALSE);
    gtk_widget_set_sensitive(stopSimButton, TRUE);
    gtk_widget_set_sensitive(executeNextButton, TRUE);
    gtk_widget_set_sensitive(executeAutoButton, TRUE);

    os.tick = 0;

    refresh_dashboard_tab();
}

void on_end_simulation_clicked(GtkButton *button, gpointer data) {
    gtk_widget_set_sensitive(startSimButton, TRUE);
    gtk_widget_set_sensitive(stopSimButton, FALSE);
    gtk_widget_set_sensitive(executeNextButton, FALSE);
    gtk_widget_set_sensitive(executeAutoButton, FALSE);

    refresh_dashboard_tab();
}

void on_reset_simulation_clicked(GtkButton *button, gpointer data) {
    gtk_widget_set_sensitive(startSimButton, TRUE);
    gtk_widget_set_sensitive(stopSimButton, FALSE);
    gtk_widget_set_sensitive(executeNextButton, FALSE);
    gtk_widget_set_sensitive(executeAutoButton, FALSE);

    Queue tqueue1;
    Queue tqueue2;
    Queue tqueue3;
    Queue tqueue4;

    initializeQueue(&tqueue1);
    initializeQueue(&tqueue2);
    initializeQueue(&tqueue3);
    initializeQueue(&tqueue4);

    queue1 = tqueue1;
    queue2 = tqueue2;
    queue3 = tqueue3;
    queue4 = tqueue4;

    memset(words, 0, sizeof(words));

    os.tick = 0;
    os.numberOfProcesses = 0;

    memset(os.processes, 0, sizeof(os.processes));
    gtk_widget_set_visible(noProcessesFound, TRUE);
    memset(os.readyQueue, 0, sizeof(os.readyQueue));
    memset(os.blockingQueue, 0, sizeof(os.blockingQueue));
    os.runningProcess = NULL;

    os.rwMutex.acquiredBy = NULL;
    os.inputMutex.acquiredBy = NULL;
    os.outputMutex.acquiredBy = NULL;
    memset(os.rwMutex.blockingQueue, 0, sizeof(os.rwMutex.blockingQueue));
    memset(os.inputMutex.blockingQueue, 0, sizeof(os.inputMutex.blockingQueue));
    memset(os.outputMutex.blockingQueue, 0, sizeof(os.outputMutex.blockingQueue));

    gtk_text_buffer_set_text(console_buffer, "", -1);
    refresh_dashboard_tab();
    refresh_table_layout();
    refresh_mutex_grid();
}

GtkWidget *create_scheduler_control_tab() {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    GtkWidget *combo_box = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "FCFS");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "Round Robin");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "MultiLevel Feedback Queue");
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_box), 0);
    g_signal_connect(combo_box, "changed", G_CALLBACK(on_combo_changed), NULL);
    gtk_box_pack_start(GTK_BOX(box), combo_box, FALSE, FALSE, 0);

    addQuantumInput(box);

    gtk_box_pack_end(GTK_BOX(box), create_execution_tab(), FALSE, TRUE, 15);

    GtkWidget *simHbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    startSimButton = gtk_button_new_with_label("Start Simulation");
    stopSimButton = gtk_button_new_with_label("Stop Simulation");
    resetSimButton = gtk_button_new_with_label("Reset Simulation");

    g_signal_connect(startSimButton, "clicked", G_CALLBACK(on_start_simulation_clicked), NULL);
    g_signal_connect(stopSimButton, "clicked", G_CALLBACK(on_end_simulation_clicked), NULL);
    g_signal_connect(resetSimButton, "clicked", G_CALLBACK(on_reset_simulation_clicked), NULL);

    gtk_box_pack_start(GTK_BOX(simHbox), startSimButton, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(simHbox), stopSimButton, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(simHbox), resetSimButton, TRUE, TRUE, 0);

    gtk_widget_set_sensitive(stopSimButton, FALSE);
    gtk_widget_set_sensitive(executeNextButton, FALSE);
    gtk_widget_set_sensitive(executeAutoButton, FALSE);

    gtk_box_pack_end(GTK_BOX(box), simHbox, FALSE, TRUE, 0);

    gtk_widget_show_all(box);
    return box;
}
