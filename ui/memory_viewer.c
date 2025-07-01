#include <gtk/gtk.h>
#include "../memory.h"

extern Word words[60];

GtkWidget* memoryViewerBox;
GtkWidget* memoryTable;

void refresh_table_layout() {
    if (memoryTable != NULL) {
        gtk_widget_destroy(memoryTable);
    }

    memoryTable = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(memoryTable), 50);

    for (int col = 0; col < 3; col++) {
        GtkWidget *inner_grid = gtk_grid_new();
        gtk_grid_set_row_spacing(GTK_GRID(inner_grid), 5);
        gtk_grid_set_column_spacing(GTK_GRID(inner_grid), 5);

        for (int i = 0; i < 20; i++) {
            const int row_index = col * 20 + i;

            char cell1_text[32], cell2_text[32];

            snprintf(cell1_text, sizeof(cell1_text), "%s", words[row_index].name == NULL ? "-" : words[row_index].name);

            if (words[row_index].type == TYPE_INT) {
                snprintf(cell2_text, sizeof(cell2_text), "%d", words[row_index].valueInt);
            } else if (words[row_index].type == TYPE_STRING) {
                snprintf(cell2_text, sizeof(cell2_text), "%s", words[row_index].valueStr);
            } else {
                snprintf(cell2_text, sizeof(cell2_text), "%s", "-");
            }

            GtkWidget *label1 = gtk_label_new(cell1_text);
            GtkWidget *label2 = gtk_label_new(cell2_text);

            GtkWidget *frame1 = gtk_frame_new(NULL);
            GtkWidget *frame2 = gtk_frame_new(NULL);
            gtk_container_add(GTK_CONTAINER(frame1), label1);
            gtk_container_add(GTK_CONTAINER(frame2), label2);

            gtk_widget_set_size_request(frame1, 150, 20);
            gtk_widget_set_size_request(frame2, 150, 20);

            gtk_grid_attach(GTK_GRID(inner_grid), frame1, 0, i, 1, 1);
            gtk_grid_attach(GTK_GRID(inner_grid), frame2, 1, i, 1, 1);
        }

        gtk_grid_attach(GTK_GRID(memoryTable), inner_grid, col, 0, 1, 1);
    }

    gtk_box_pack_start(GTK_BOX(memoryViewerBox), memoryTable, TRUE, TRUE, 20);

    gtk_widget_set_halign(memoryTable, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(memoryTable, GTK_ALIGN_CENTER);

    gtk_widget_show_all(memoryViewerBox);
}

GtkWidget *create_memory_viewer_tab() {
    memoryViewerBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    refresh_table_layout();
    gtk_widget_show_all(memoryViewerBox);
    return memoryViewerBox;
}
