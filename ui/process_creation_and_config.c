#include <gtk/gtk.h>

#include "dashboard.h"
#include "../os.h"
#include "../memory.h"

extern OS os;
extern Word words[60];
extern GtkWindow *window;
extern GtkWidget *dashboardBox;

char code[8192] = {0};

GtkWidget *arrivalTimeLabel;
GtkWidget *arrivalTimeEntry;
GtkWidget *priorityLabel;
GtkWidget *priorityEntry;
GtkWidget *addProcessButton;
GtkWidget *scrolled_window;

void addProcess(GtkButton *button, GtkWidget *parentBox) {
    const char *input_text = gtk_entry_get_text(GTK_ENTRY(arrivalTimeEntry));
    char *endptr;
    const long arrivalTicks = strtol(input_text, &endptr, 10);
    if (endptr == input_text) {
        g_message("Invalid input\n");
        return;
    }

    const char *input_text1 = gtk_entry_get_text(GTK_ENTRY(priorityEntry));
    const char *endPtr1 = 0;
    const long priority = strtol(input_text1, &endptr, 10);
    if (endPtr1 == input_text1) {
        g_message("Invalid input\n");
        return;
    }

    const int pid = os.numberOfProcesses;
    os.numberOfProcesses++;

    const int initialProcessCodeWord = (20 * pid) + CODE_OFFSET;

    os.processes[pid] = (PCB*) malloc(sizeof(PCB));
    os.processes[pid]->processID = pid;
    os.processes[pid]->processState = "READY";
    os.processes[pid]->priority = (int) priority;
    os.processes[pid]->programCounter = initialProcessCodeWord;
    os.processes[pid]->memoryBoundaries[0] = 20 * pid;
    os.processes[pid]->memoryBoundaries[1] = 20 * pid + 19;
    os.processes[pid]->arriveTick = (int) arrivalTicks;
    os.processes[pid]->code = g_strdup(code);

    gtk_widget_destroy(arrivalTimeLabel);
    gtk_widget_destroy(arrivalTimeEntry);
    gtk_widget_destroy(priorityLabel);
    gtk_widget_destroy(priorityEntry);
    gtk_widget_destroy(addProcessButton);
    gtk_widget_destroy(scrolled_window);

    refresh_dashboard_tab();
}

void addArrivalTimeInput(GtkWidget *parentBox) {
    arrivalTimeLabel = gtk_label_new("Insert arrival time (in ticks)");

    arrivalTimeEntry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(arrivalTimeEntry), "1");

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(box), arrivalTimeLabel, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), arrivalTimeEntry, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(parentBox), box, FALSE, FALSE, 0);

    addProcessButton = gtk_button_new_with_label("Add process");
    g_signal_connect(addProcessButton, "clicked", G_CALLBACK(addProcess), parentBox);
    gtk_box_pack_end(GTK_BOX(parentBox), addProcessButton, FALSE, FALSE, 0);

    gtk_widget_show_all(parentBox);
}

void addPriorityInput(GtkWidget *parentBox) {
    priorityLabel = gtk_label_new("Insert priority");
    priorityEntry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(priorityEntry), "1");

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(box), priorityLabel, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), priorityEntry, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(parentBox), box, FALSE, FALSE, 0);

    gtk_widget_show_all(parentBox);
}

void addTextEditor(GtkWidget *parentBox, const char *text) {
    GtkWidget *text_view = gtk_text_view_new();
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_set_text(buffer, text, -1);

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
                                    "textview {"
                                    "  background-color: #1e1e1e;"
                                    "  color: #d4d4d4;"
                                    "  font-family: monospace;"
                                    "  font-size: 12pt;"
                                    "  padding: 10px;"
                                    "  border: 2px solid #ffffff;" // White border
                                    "  border-radius: 5px;" // Rounded corners (optional)
                                    "}", -1, NULL);

    GtkStyleContext *context = gtk_widget_get_style_context(text_view);
    gtk_style_context_add_provider(context,
                                   GTK_STYLE_PROVIDER(provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);

    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);

    gtk_widget_set_size_request(scrolled_window, 50, 200);

    gtk_box_pack_start(GTK_BOX(parentBox), scrolled_window, FALSE, FALSE, 0);

    addArrivalTimeInput(parentBox);
    addPriorityInput(parentBox);
}

void on_open_program_clicked(GtkButton *button, const gpointer user_data) {
    GtkWidget *box = GTK_WIDGET(user_data);

    GtkWidget *dialog = gtk_file_chooser_dialog_new("Open File",
                                                    window,
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Open", GTK_RESPONSE_ACCEPT,
                                                    NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        const char *filename = gtk_file_chooser_get_filename(chooser);
        FILE *file = fopen(filename, "r");

        memset(code, 0, sizeof(code));

        char text[512];
        while (fgets(text, sizeof(text), file)) {
            text[strcspn(text, "\n")] = 0;
            strcat(code, text);
            strcat(code, "\n");
        }
        fclose(file);
        addTextEditor(box, code);
    }

    gtk_widget_destroy(dialog);
}

GtkWidget *create_process_creation_and_config_tab() {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    GtkWidget *openProgramButton = gtk_button_new_with_label("Open program");
    g_signal_connect(openProgramButton, "clicked", G_CALLBACK(on_open_program_clicked), box);

    gtk_box_pack_start(GTK_BOX(box), openProgramButton, FALSE, FALSE, 0);

    return box;
}
