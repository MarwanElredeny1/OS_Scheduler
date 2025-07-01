#include <gtk/gtk.h>
#include "../os.h"

extern OS os;

GtkWidget* numberOfProcessesLabel;
GtkWidget* currentClockCycleLabel;
GtkWidget* selectedSchedulerLabel;
GtkWidget* processes_scroll_window;
GtkWidget* processesList;
GtkWidget* noProcessesFound;

GtkWidget* dashboardBox;

void refresh_dashboard_tab() {
    gtk_label_set_label(
        GTK_LABEL(numberOfProcessesLabel), g_strdup_printf("Total Number of Processes: %d", os.numberOfProcesses));
    gtk_label_set_label(GTK_LABEL(currentClockCycleLabel), g_strdup_printf("Current Clock Cycle: %d", os.tick));
    gtk_label_set_label(GTK_LABEL(selectedSchedulerLabel), g_strdup_printf("Selected Scheduling Algorithm: %s",
                                                                           os.selectedScheduler == FCFS
                                                                               ? "FCFS"
                                                                               : os.selectedScheduler == RoundRobin
                                                                                       ? "Round Robin"
                                                                                       : "MultiLevel Feedback Model"));

    if (processes_scroll_window != NULL) {
        gtk_widget_destroy(processes_scroll_window);
    }

    if (os.numberOfProcesses > 0) {
        processes_scroll_window = gtk_scrolled_window_new(NULL, NULL);
        gtk_widget_set_size_request(processes_scroll_window, 1, 200);

        processesList = gtk_list_box_new();
        gtk_container_add(GTK_CONTAINER(processes_scroll_window), processesList);

        for (int i = 0; i < os.numberOfProcesses; i++) {
            PCB *pcb = os.processes[i];
            GtkWidget *label = gtk_label_new(pcbAsString(pcb));

            gtk_widget_set_halign(label, GTK_ALIGN_START);
            gtk_list_box_insert(GTK_LIST_BOX(processesList), label, -1);

            gtk_box_pack_start(GTK_BOX(dashboardBox), processes_scroll_window, FALSE, FALSE, 15);
        }
    }
    gtk_widget_show_all(dashboardBox);
    gtk_widget_set_visible(noProcessesFound, os.numberOfProcesses == 0);
}

GtkWidget *create_dashboard_tab() {
    dashboardBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    // General Information
    numberOfProcessesLabel = gtk_label_new("1");
    currentClockCycleLabel = gtk_label_new("2");
    selectedSchedulerLabel = gtk_label_new("3");

    gtk_widget_set_halign(numberOfProcessesLabel, GTK_ALIGN_START);
    gtk_widget_set_halign(currentClockCycleLabel, GTK_ALIGN_START);
    gtk_widget_set_halign(selectedSchedulerLabel, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(dashboardBox), numberOfProcessesLabel, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(dashboardBox), currentClockCycleLabel, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(dashboardBox), selectedSchedulerLabel, FALSE, FALSE, 5);

    // Showing all PCBs of detected processes
    noProcessesFound = gtk_label_new(g_strdup_printf("No processes added to the system"));
    gtk_widget_set_halign(noProcessesFound, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(dashboardBox), noProcessesFound, FALSE, FALSE, 5);

    refresh_dashboard_tab();

    return dashboardBox;
}
