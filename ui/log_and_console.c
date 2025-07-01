#include <ctype.h>
#include <gtk/gtk.h>

#include "dashboard.h"
#include "memory_viewer.h"
#include "resource_management.h"
#include "visualization_execution.h"
#include "../os.h"
#include "../memory.h"
#include "../schedulers/queue.h"

GtkWidget *input_entry = NULL;
GtkTextBuffer *console_buffer = NULL;

extern GtkWidget *executeNextButton;
extern GtkWidget *executeAutoButton;

extern OS os;
extern Word words[60];
extern char *variableName;
extern bool isTicking;
extern bool isAutoExecute;

extern Queue queue1;
PCB *pcb_waiting_for_input = NULL;

bool is_number(const char *str) {
    char *endptr;
    double val = strtod(str, &endptr);

    if (str == endptr) return false;

    while (*endptr != '\0') {
        if (!isspace((unsigned char)*endptr)) return false;
        endptr++;
    }

    return true;
}

static void on_entry_activate(GtkEntry *entry, const gpointer user_data) {
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(user_data);
    const gchar *text = gtk_entry_get_text(entry);

    if (g_strcmp0(text, "") == 0) return;

    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);

    if (pcb_waiting_for_input != NULL) {
        for (int j = 20 * pcb_waiting_for_input->processID + VARIABLES_OFFSET;
             j < 20 * pcb_waiting_for_input->processID + VARIABLES_OFFSET + 5; j++) {
            if (words[j].name == NULL) {
                char *value = g_strdup(text);
                words[j].name = variableName;

                if (is_number(value)) {
                    words[j].valueInt = atoi(value);
                    words[j].type = TYPE_INT;
                } else {
                    words[j].valueStr = value;
                    words[j].type = TYPE_STRING;
                }
                break;
            }
        }

        pcb_waiting_for_input = NULL;
        variableName = NULL;

        gtk_text_buffer_insert(buffer, &end, text, -1);
        gtk_text_buffer_insert(buffer, &end, "\n", -1);

        gtk_entry_set_text(entry, "");
        refresh_dashboard_tab();
        refresh_table_layout();
        refresh_mutex_grid();

        isTicking = true;
        if (isAutoExecute) on_auto_execution_clicked(NULL, NULL);
    }
}

GtkWidget *create_log_and_console_tab() {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    gtk_box_pack_end(GTK_BOX(box), create_execution_tab(), FALSE, TRUE, 15);
    // gtk_widget_set_sensitive(executeNextButton, FALSE);
    // gtk_widget_set_sensitive(executeAutoButton, FALSE);

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_vexpand(scrolled_window, TRUE);

    GtkWidget *text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text_view), FALSE);

    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);

    GtkWidget *input_entry = gtk_entry_new();

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    g_signal_connect(input_entry, "activate", G_CALLBACK(on_entry_activate), buffer);
    gtk_text_buffer_create_tag(buffer, "red_fg", "foreground", "red", NULL);
    gtk_text_buffer_create_tag(buffer, "green_fg", "foreground", "green", NULL);
    gtk_text_buffer_create_tag(buffer, "blue_fg", "foreground", "blue", NULL);
    console_buffer = buffer;

    gtk_box_pack_start(GTK_BOX(box), scrolled_window, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), input_entry, FALSE, FALSE, 0);

    return box;
}
