#include <gtk/gtk.h>
#include "gtk_widgets.h"
#include "ttt.h"

GtkWidget *main_window;
GtkWidget *grid;
GtkWidget *layout;
GtkWidget *backgnd;
GtkWidget *button;
GtkWidget *space;
GtkWidget *label;
GtkWidget *player1;
GtkWidget *player2;
GtkWidget *replay_btn;
GtkWidget *main_menu_btn;
GtkWidget *left_label;
GtkWidget *right_label;
GtkWidget *x_label;
GtkWidget *o_label;
GtkWidget *square_btn[9];
GtkWidget *left_score_label;
GtkWidget *right_score_label;
GtkWidget *tie_label;
GtkWidget *tie_score_label;

const char *player1_name;
const char *player2_name;

void clear_layout()
{
    GtkWidget *childs;
    childs = gtk_widget_get_first_child(layout);
    while (childs != NULL)
    {
        gtk_fixed_remove(GTK_FIXED(layout), childs);
        childs = gtk_widget_get_first_child(layout);
    }
}

/* Function to show dialog */
void showdialog(const char *title, const char *message)
{
    GtkWidget *dialog, *label, *content_area;
    GtkDialogFlags flags;

    // Create the widgets for dialog
    flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_dialog_new_with_buttons(title, GTK_WINDOW(main_window), flags, "OK", GTK_RESPONSE_NONE, NULL);

    GtkWidget *btn = gtk_widget_get_last_child(gtk_widget_get_last_child(dialog)); // last child of dialog is content area, last child of content area is button to be aligned
    gtk_widget_set_halign(btn, GTK_ALIGN_CENTER);

    gtk_widget_set_size_request(dialog, 250, 30);
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    label = gtk_label_new("");
    gtk_label_set_markup(GTK_LABEL(label), g_strdup_printf("<span size = 'large'>%s</span>", message)); // message is showed as label
    // Ensure that the dialog box is destroyed when the user responds

    g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);

    // Add the label, and show everything

    gtk_box_append(GTK_BOX(content_area), label);

    gtk_widget_show(dialog);
}

void placeLabel(GtkWidget *widget, int x, int y, int width, int height, float xalign, GtkAlign valign)
{
    gtk_label_set_xalign(GTK_LABEL(widget), xalign);
    gtk_widget_set_valign(widget, GTK_ALIGN_CENTER);
    gtk_fixed_put(GTK_FIXED(layout), widget, x, y);
    gtk_widget_set_size_request(widget, width, height);
}

void placeWidget(GtkWidget *widget, int x, int y, int width, int height)
{
    gtk_fixed_put(GTK_FIXED(layout), widget, x, y);
    gtk_widget_set_size_request(widget, width, height);
}
