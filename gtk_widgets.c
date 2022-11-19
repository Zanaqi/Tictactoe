#include <gtk/gtk.h>
#include "gtk_widgets.h"
#include "ttt.h"

GtkWidget *main_window;
GtkWidget *layout;

void clear_layout(char *backgnd_resource)
{
    GtkWidget *childs, *backgnd;

    /* Remove current child in layout*/
    childs = gtk_widget_get_first_child(layout);
    while (childs != NULL)
    {
        gtk_fixed_remove(GTK_FIXED(layout), childs);
        childs = gtk_widget_get_first_child(layout);
    }

    /*Set background image*/
    backgnd = gtk_image_new_from_file(backgnd_resource);
    gtk_widget_set_size_request(backgnd, 720, 720);
    gtk_fixed_put(GTK_FIXED(layout), backgnd, 0, 0);
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

void placeWidget(GtkWidget *widget, int x, int y, int width, int height)
{
    gtk_fixed_put(GTK_FIXED(layout), widget, x, y);
    gtk_widget_set_size_request(widget, width, height);
}

void labels_state(int state, struct Gamedata *gamedata)
{

    if (state == 1) //  bold player 1, grey player 2
    {
        // bold player 1
        gtk_label_set_markup(GTK_LABEL(gamedata->left_label), g_strdup_printf("<span size = 'x-large'><b>%s</b></span>", gamedata->player1_name));
        gtk_label_set_markup(GTK_LABEL(gamedata->x_label), "<span size = 'x-large'><b><u>Player 1 (X)</u></b></span>");

        // grey player 2
        gtk_label_set_markup(GTK_LABEL(gamedata->right_label), g_strdup_printf("<span size = 'x-large' foreground = 'grey'>%s</span>", gamedata->player2_name));
        gtk_label_set_markup(GTK_LABEL(gamedata->o_label), "<span size = 'x-large' foreground = 'grey'><u>Player 2 (O)</u></span>");
    }
    else if (state == 2) // grey player 1, bold player 2
    {
        // grey player 1
        gtk_label_set_markup(GTK_LABEL(gamedata->left_label), g_strdup_printf("<span size = 'x-large' foreground = 'grey'>%s</span>", gamedata->player1_name));
        gtk_label_set_markup(GTK_LABEL(gamedata->x_label), "<span size = 'x-large' foreground = 'grey'><u>Player 1 (X)</u></span>");

        // bold player 2
        gtk_label_set_markup(GTK_LABEL(gamedata->right_label), g_strdup_printf("<span size = 'x-large'><b>%s</b></span>", gamedata->player2_name));
        gtk_label_set_markup(GTK_LABEL(gamedata->o_label), "<span size = 'x-large'><b><u>Player 2 (O)</u></b></span>");
    }
    else if (state == 3) // player 1 and player 2 no markup
    {
        // player 1
        gtk_label_set_markup(GTK_LABEL(gamedata->left_label), g_strdup_printf("<span size = 'x-large'>%s</span>", gamedata->player1_name));
        gtk_label_set_markup(GTK_LABEL(gamedata->x_label), "<span size = 'x-large'><u>Player 1 (X)</u></span>");

        // player 2
        gtk_label_set_markup(GTK_LABEL(gamedata->right_label), g_strdup_printf("<span size = 'x-large'>%s</span>", gamedata->player2_name));
        gtk_label_set_markup(GTK_LABEL(gamedata->o_label), "<span size = 'x-large'><u>Player 2 (O)</u></span>");
    }
    else if (state == 4) // grey player 1 and grey player 2
    {
        // grey player 1
        gtk_label_set_markup(GTK_LABEL(gamedata->left_label), g_strdup_printf("<span size = 'x-large' foreground = 'grey'>%s</span>", gamedata->player1_name));
        gtk_label_set_markup(GTK_LABEL(gamedata->x_label), "<span size = 'x-large' foreground = 'grey'><u>Player 1 (X)</u></span>");

        // grey player 2
        gtk_label_set_markup(GTK_LABEL(gamedata->right_label), g_strdup_printf("<span size = 'x-large' foreground = 'grey'>%s</span>", gamedata->player2_name));
        gtk_label_set_markup(GTK_LABEL(gamedata->o_label), "<span size = 'x-large' foreground = 'grey'><u>Player 2 (O)</u></span>");
    }
    else if (state == 5) // grey score labels
    {
        gtk_label_set_markup(GTK_LABEL(gamedata->left_score_label), g_strdup_printf("<span size = 'x-large' foreground = 'grey'>%d</span>", gamedata->player1_score));
        gtk_label_set_markup(GTK_LABEL(gamedata->right_score_label), g_strdup_printf("<span size = 'x-large' foreground = 'grey'>%d</span>", gamedata->player2_score));
        gtk_label_set_markup(GTK_LABEL(gamedata->tie_score_label), g_strdup_printf("<span size = 'x-large' foreground = 'grey'>%d</span>", gamedata->tie_score));
        gtk_label_set_markup(GTK_LABEL(gamedata->tie_label), "<span size = 'x-large' foreground = 'grey'>Tie</span>");
    }
}