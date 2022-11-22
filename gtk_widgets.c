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
    gtk_widget_set_size_request(dialog, 300, 80);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    label = gtk_label_new("");
    gtk_label_set_text(GTK_LABEL(label), message); // message is showed as label
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
    gchar *first_text, *second_text, *third_text, *fourth_text;

    switch (state)
    {
    case 1:
        // bold player 1, grey player 2
        first_text = g_strdup_printf("<span size = 'large'><b>%s</b></span>", gamedata->player1_name);
        second_text = g_strdup("<span size = 'large'><b><u>Player 1 (X)</u></b></span>");
        third_text = g_strdup_printf("<span size = 'large' foreground = 'grey'>%s</span>", gamedata->player2_name);
        fourth_text = g_strdup("<span size = 'large' foreground = 'grey'><u>Player 2 (O)</u></span>");
        break;

    case 2:
        // grey player 1, bold player 2
        first_text = g_strdup_printf("<span size = 'large' foreground = 'grey'>%s</span>", gamedata->player1_name);
        second_text = g_strdup("<span size = 'large' foreground = 'grey'><u>Player 1 (X)</u></span>");
        third_text = g_strdup_printf("<span size = 'large'><b>%s</b></span>", gamedata->player2_name);
        fourth_text = g_strdup("<span size = 'large'><b><u>Player 2 (O)</u></b></span>");
        break;

    case 3:
        // player 1 and 2 no difference
        first_text = g_strdup_printf("<span size = 'large'>%s</span>", gamedata->player1_name);
        second_text = g_strdup("<span size = 'large'><u>Player 1 (X)</u></span>");
        third_text = g_strdup_printf("<span size = 'large'>%s</span>", gamedata->player2_name);
        fourth_text = g_strdup("<span size = 'large'><u>Player 2 (O)</u></span>");
        break;

    case 4:
        // grey both player 1 and 2
        first_text = g_strdup_printf("<span size = 'large' foreground = 'grey'>%s</span>", gamedata->player1_name);
        second_text = g_strdup("<span size = 'large' foreground = 'grey'><u>Player 1 (X)</u></span>");
        third_text = g_strdup_printf("<span size = 'large' foreground = 'grey'>%s</span>", gamedata->player2_name);
        fourth_text = g_strdup("<span size = 'large' foreground = 'grey'><u>Player 2 (O)</u></span>");
        break;

    case 5:
        // grey score labels
        first_text = g_strdup_printf("<span size = 'large' foreground = 'grey'>%d</span>", gamedata->player1_score);
        second_text = g_strdup_printf("<span size = 'large' foreground = 'grey'>%d</span>", gamedata->player2_score);
        third_text = g_strdup_printf("<span size = 'large' foreground = 'grey'>%d</span>", gamedata->tie_score);
        fourth_text = g_strdup("<span size = 'large' foreground = 'grey'>Tie</span>");
        break;
    }

    switch (state) // set label
    {
    case 1:
    case 2:
    case 3:
    case 4:
        gtk_label_set_markup(GTK_LABEL(gamedata->left_label), first_text);
        gtk_label_set_markup(GTK_LABEL(gamedata->x_label), second_text);
        gtk_label_set_markup(GTK_LABEL(gamedata->right_label), third_text);
        gtk_label_set_markup(GTK_LABEL(gamedata->o_label), fourth_text);
        break;

    case 5:
        gtk_label_set_markup(GTK_LABEL(gamedata->left_score_label), first_text);
        gtk_label_set_markup(GTK_LABEL(gamedata->right_score_label), second_text);
        gtk_label_set_markup(GTK_LABEL(gamedata->tie_score_label), third_text);
        gtk_label_set_markup(GTK_LABEL(gamedata->tie_label), fourth_text);
        break;
    }
    g_free(first_text);
    g_free(second_text);
    g_free(third_text);
    g_free(fourth_text);
}