#include <gtk/gtk.h>

GtkWidget* create_visualized_execution_buttons() {
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    GtkWidget* button1 = gtk_button_new();
    GtkWidget* icon = gtk_image_new_from_icon_name("media-playback-start-symbolic", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image(GTK_BUTTON(button1), icon);
    gtk_widget_set_tooltip_text(button1, "advancing the simulation one step at a time (one clock cycle)");
    gtk_box_pack_start(GTK_BOX(box), button1, TRUE, TRUE, 0);

    GtkWidget* button2 = gtk_button_new_with_label("Execute automatically");
    gtk_widget_set_tooltip_text(button2, " continuous execution until all processes are completed or stopped");
    gtk_box_pack_start(GTK_BOX(box), button2, TRUE, TRUE, 0);
    return box;
}