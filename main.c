#include <stdio.h>
#include "memory.h"
#include "os.h"
#include <gtk/gtk.h>

#include "schedulers/queue.h"
#include "ui/dashboard.h"
#include "ui/log_and_console.h"
#include "ui/memory_viewer.h"
#include "ui/process_creation_and_config.h"
#include "ui/resource_management.h"
#include "ui/scheduler_control.h"

OS os;
Word words[60];

Queue queue1;
Queue queue2;
Queue queue3;
Queue queue4;

GtkWindow* window;

int main(int argc, char* argv[]){
    initializeQueue(&queue1);
    initializeQueue(&queue2);
    initializeQueue(&queue3);
    initializeQueue(&queue4);

    os.tick = 0;
    os.quantum = 2;
    os.selectedScheduler = FCFS;
    os.numberOfProcesses = 0;

    gtk_init(&argc, &argv);

    window = (GtkWindow*) gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Task Manager Tabs");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

    GtkWidget* notebook = gtk_notebook_new();

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), create_dashboard_tab(), gtk_label_new("Main Dashboard"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), create_log_and_console_tab(), gtk_label_new("Log & Console Panel"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), create_memory_viewer_tab(), gtk_label_new("Memory Viewer"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), create_process_creation_and_config_tab(), gtk_label_new("Process Creation and Configuration"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), create_resource_management_tab(), gtk_label_new("Resource Management Panel"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), create_scheduler_control_tab(), gtk_label_new("Scheduler Control Panel"));

    gtk_container_add(GTK_CONTAINER(window), notebook);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all((GtkWidget*) window);

    gtk_main();

    return 0;
}