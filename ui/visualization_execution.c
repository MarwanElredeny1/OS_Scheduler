#include <stdbool.h>
#include <gtk/gtk.h>
#include "../os.h"
#include "dashboard.h"
#include "memory_viewer.h"
#include "resource_management.h"

extern OS os;

GtkWidget *executeNextButton;
GtkWidget *executeAutoButton;

bool isTicking = true;
bool isAutoExecute = false;

void on_next_execution_clicked(GtkButton *button, gpointer data) {
    os.tick++;
    tick();

    refresh_dashboard_tab();
    refresh_table_layout();
    refresh_mutex_grid();
}

bool is_processes_completed() {
    for (int i = 0; i < 3; i++) {
        if (os.processes[i] != NULL && strcmp(os.processes[i]->processState, "TERMINATED") != 0) return false;
    }

    return true;
}

void on_auto_execution_clicked(GtkButton *button, const gpointer data) {
    isAutoExecute = true;
    while (!is_processes_completed()) {
        if (!isTicking) break;
        on_next_execution_clicked(button, data);
    }
}

GtkWidget *create_execution_tab() {
    GtkWidget *executionBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    executeNextButton = gtk_button_new();
    GtkWidget *icon = gtk_image_new_from_icon_name("media-playback-start-symbolic", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image(GTK_BUTTON(executeNextButton), icon);
    gtk_widget_set_tooltip_text(executeNextButton, "advancing the simulation one step at a time (one clock cycle)");
    gtk_box_pack_start(GTK_BOX(executionBox), executeNextButton, TRUE, TRUE, 0);
    g_signal_connect(executeNextButton, "clicked", G_CALLBACK(on_next_execution_clicked), NULL);

    executeAutoButton = gtk_button_new_with_label("Execute automatically");
    gtk_widget_set_tooltip_text(executeAutoButton,
                                " continuous execution until all processes are completed or stopped");
    gtk_box_pack_start(GTK_BOX(executionBox), executeAutoButton, TRUE, TRUE, 0);
    g_signal_connect(executeAutoButton, "clicked", G_CALLBACK(on_auto_execution_clicked), NULL);

    return executionBox;
}
