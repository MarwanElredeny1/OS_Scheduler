#ifndef VISUALIZATION_EXECUTION_H
#define VISUALIZATION_EXECUTION_H

#include <stdbool.h>
#include <gtk/gtk.h>

void on_auto_execution_clicked(GtkButton *button, const gpointer data);

GtkWidget *create_execution_tab();

#endif