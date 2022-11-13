#include <gtk/gtk.h>
#include "ai.h"
#include "ttt.h"
#include "gtk_widgets.h"
#include "minimax.h"

void test_data();

static void two_player(GtkWidget *widget, gpointer data);
static void two_player_gamescreen(GtkWidget *widget, gpointer data);

static void singleplayer(GtkWidget *widget, gpointer data);
void singleplayer_gamescreen();
void computer_move(int player_move);

static void draw_function(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer data);
void gamescreen();

void winning_line(char winner);
static void square_clicked(GtkWidget *widget, gpointer data);

void end_game(int result);
static void replay(GtkWidget *widget, gpointer data);
static void restart(GtkWidget *widget, gpointer data);

void main_layout();
static void main_menu(GtkWidget *widget, gpointer data);
static void setup(GtkApplication *app, gpointer user_data);

int main();

void test_data()
{
    double positive_prior = total_positive / 958.0;
    double negative_prior = total_negative / 958.0;

    int positive_testing = total_positive * 0.2; // number of test positive
    int negative_testing = total_negative * 0.2; // number of test negative
    int confusion_matrix[4] = {0, 0, 0, 0};      // tp, fn, fp, tn
    int k = 0;                                   // values for confusion matrix

    int positive = total_positive * 0.8;
    float total_testing = positive_testing + negative_testing;
    float error = 0.0, accuracy = 0.0;
    double p_probability, n_probability, p_posterior, n_posterior;

    // test positive results
    for (int i = 0; i < positive_testing; i++)
    {
        p_probability = positive_prior;
        n_probability = negative_prior;

        /*for each element in dataset, get posterior probability and multiply with the prior probability*/

        for (k = 0; k < 9; k++)
        {
            if (dataset[i + positive][k] == 'x')
            {
                p_probability = p_probability * conditional_x[k + 9];
                n_probability = n_probability * conditional_x[k];
            }

            else if (dataset[i + positive][k] == 'o')
            {
                p_probability = p_probability * conditional_o[k + 9];
                n_probability = n_probability * conditional_o[k];
            }
            else if (dataset[i + positive][k] == 'b')
            {
                p_probability = p_probability * conditional_b[k + 9];
                n_probability = n_probability * conditional_b[k];
            }
        }

        if (p_probability > n_probability) // True positive
            confusion_matrix[0]++;

        else
        {
            error++;               // increase count of error output
            confusion_matrix[1]++; // False Negative
        }
    }

    // test negative results
    for (int i = TOTAL - 1; i > TOTAL - negative_testing; i--)
    {
        p_probability = positive_prior;
        n_probability = negative_prior;

        /*for each element in dataset, get posterior probability and multiply with the prior probability*/

        for (k = 0; k < 9; k++)
        {
            if (dataset[i][k] == 'x')
            {
                p_probability = p_probability * conditional_x[k + 9];
                n_probability = n_probability * conditional_x[k];
            }

            else if (dataset[i][k] == 'o')
            {
                p_probability = p_probability * conditional_o[k + 9];
                n_probability = n_probability * conditional_o[k];
            }
            else if (dataset[i][k] == 'b')
            {
                p_probability = p_probability * conditional_b[k + 9];
                n_probability = n_probability * conditional_b[k];
            }
        }

        if (p_probability > n_probability)
        {
            error++;               // increase count of error
            confusion_matrix[2]++; // False Positive
        }

        else
            confusion_matrix[3]++; // True Negative
    }

    accuracy = ((total_testing - error) / total_testing) * 100;
    g_print("\nTesting data with 20%% of data set\n\n");
    g_print("Total tested                    %1.0f\n", total_testing);
    g_print("True Positive (TP)              %d\n", confusion_matrix[0]);
    g_print("False Negative (FN)             %d\n", confusion_matrix[1]);
    g_print("False Positive (FP)             %d\n", confusion_matrix[2]);
    g_print("True Negative (TN)              %d\n", confusion_matrix[3]);
    g_print("Number of errors                %1.0f\n", error);
    g_print("Accuracy                        %0.2f%%\n", accuracy);
}

/* Set up widgets for two player game */
/* Change window title, let users key in names and set up start button to start game */
static void two_player(GtkWidget *widget, gpointer data)
{
    gtk_window_set_title(GTK_WINDOW(main_window), "Two Player"); // change window title
    clear_layout();

    backgnd = gtk_image_new_from_file("resources/main-backgnd.png");
    gtk_widget_set_size_request(backgnd, 720, 720);
    gtk_fixed_put(GTK_FIXED(layout), backgnd, 0, 0);

    label = gtk_label_new("");
    gtk_label_set_markup(GTK_LABEL(label), "<span size = 'large'>Enter Names (Max 10 characters)</span>");
    placeLabel(label, 190, 200, 340, 30, 0.5, GTK_ALIGN_CENTER);

    /* Entries for players to key in names for labels in the game screen */
    player1 = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(player1), "Player 1");
    placeWidget(player1, 280, 250, BUTTON_WIDTH, BUTTON_HEIGHT);

    player2 = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(player2), "Player 2");
    placeWidget(player2, 280, 310, BUTTON_WIDTH, BUTTON_HEIGHT);

    /* Start button */
    button = gtk_button_new_with_label("");
    g_signal_connect(button, "clicked", G_CALLBACK(two_player_gamescreen), NULL);
    gtk_label_set_markup(GTK_LABEL(gtk_widget_get_first_child(button)), "<span size = 'large' foreground = 'green'><b>Start</b></span>");
    placeWidget(button, 280, 370, BUTTON_WIDTH, BUTTON_HEIGHT);

    /* Back button to go back to main menu */
    button = gtk_button_new_with_label("");
    g_signal_connect(button, "clicked", G_CALLBACK(main_menu), NULL);
    gtk_label_set_markup(GTK_LABEL(gtk_widget_get_first_child(button)), "<span size = 'large'><b>Back</b></span>");
    placeWidget(button, 50, 630, 100, 40);
}

/* initialise left and right labels (cannot attach yet as gamescreen function will remove all previous widgets) */
static void two_player_gamescreen(GtkWidget *widget, gpointer data)
{
    GtkWidget *player1_label, *player2_label;

    if (gtk_entry_buffer_get_length(gtk_entry_get_buffer(GTK_ENTRY(player1))) > 10)
    {
        showdialog("Too many characters", "Please enter name with less than 10 characters");
        two_player(NULL, NULL);
    }
    else if (gtk_entry_buffer_get_length(gtk_entry_get_buffer(GTK_ENTRY(player2))) > 10)
    {
        showdialog("Too many characters", "Please enter name with less than 10 characters");
        two_player(NULL, NULL);
    }
    else
    {
        current_player = 1;
        starting_player = 1;

        /* Get player nammes from previous screen and set label names */
        player1_name = g_strdup(gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(player1))));
        player2_name = g_strdup(gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(player2))));

        left_label = gtk_label_new("");
        gtk_label_set_markup(GTK_LABEL(left_label), g_strdup_printf("<span size = 'x-large'><b>%s</b></span>", player1_name));

        right_label = gtk_label_new("");
        gtk_label_set_markup(GTK_LABEL(right_label), g_strdup_printf("<span size = 'x-large' foreground = 'grey'>%s</span>", player2_name));
        gamescreen(); // standard gamescreen
    }
}

/* Screen where user chooses difficulty before starting single player game */
/* Change window title, insert three buttons for three difficulty and include a back button*/
static void single_player(GtkWidget *widget, gpointer data)
{
    int *hard_ptr = malloc(sizeof(int));
    *hard_ptr = 3;

    int *med_ptr = malloc(sizeof(int));
    *med_ptr = 2;

    int *easy_ptr = malloc(sizeof(int));
    *easy_ptr = 1;

    gtk_window_set_title(GTK_WINDOW(main_window), "Single Player"); // change window title

    clear_layout();

    backgnd = gtk_image_new_from_file("resources/main-backgnd.png");
    gtk_widget_set_size_request(backgnd, 720, 720);
    gtk_fixed_put(GTK_FIXED(layout), backgnd, 0, 0);

    label = gtk_label_new("Choose your difficulty");
    gtk_label_set_markup(GTK_LABEL(label), "<span size = 'large'>Choose your difficulty</span>");
    placeLabel(label, 260, 200, 200, 30, 0.5, GTK_ALIGN_CENTER);

    /* Difficulty buttons, each button links to different callback functions to change global variable 'difficulty' to change how computer plays*/
    button = gtk_button_new_with_label("");
    g_signal_connect(button, "clicked", G_CALLBACK(singleplayer_gamescreen), easy_ptr);
    gtk_label_set_markup(GTK_LABEL(gtk_widget_get_first_child(button)), "<span size = 'large' foreground = 'green'><b>Easy</b></span>");
    placeWidget(button, 280, 250, BUTTON_WIDTH, BUTTON_HEIGHT);

    button = gtk_button_new_with_label("");
    g_signal_connect(button, "clicked", G_CALLBACK(singleplayer_gamescreen), med_ptr);
    gtk_label_set_markup(GTK_LABEL(gtk_widget_get_first_child(button)), "<span size = 'large' foreground = 'orange'><b>Medium</b></span>");
    placeWidget(button, 280, 310, BUTTON_WIDTH, BUTTON_HEIGHT);

    button = gtk_button_new_with_label("");
    g_signal_connect(button, "clicked", G_CALLBACK(singleplayer_gamescreen), hard_ptr);
    gtk_label_set_markup(GTK_LABEL(gtk_widget_get_first_child(button)), "<span size = 'large' foreground = 'red'><b>Hard</b></span>");
    placeWidget(button, 280, 370, BUTTON_WIDTH, BUTTON_HEIGHT);

    /* Back button to go back to main menu*/
    button = gtk_button_new_with_label("");
    g_signal_connect(button, "clicked", G_CALLBACK(main_menu), NULL);
    gtk_label_set_markup(GTK_LABEL(gtk_widget_get_first_child(button)), "<span size = 'large'><b>Back</b></span>");
    placeWidget(button, 50, 630, 100, 40);
}

/* initialise left and right labels (cannot attach yet as gamescreen function will remove all previous widgets) */
void singleplayer_gamescreen(GtkWidget *widget, gpointer data)
{
    difficulty = *(int *)data;

    if (difficulty == 1)
        gtk_window_set_title(GTK_WINDOW(main_window), "Single Player - Easy");
    else if (difficulty == 2)
        gtk_window_set_title(GTK_WINDOW(main_window), "Single Player - Medium");
    else if (difficulty == 3)
        gtk_window_set_title(GTK_WINDOW(main_window), "Single Player - Hard");

    current_player = 3;
    starting_player = 1;

    left_label = gtk_label_new(""); // player, x
    gtk_label_set_markup(GTK_LABEL(left_label), "<span size = 'x-large'>You</span>");

    right_label = gtk_label_new("Computer"); // computer, o
    gtk_label_set_markup(GTK_LABEL(right_label), "<span size = 'x-large'>Computer</span>");

    gamescreen();
}

/* Called after every player move and if game is continuing */
void computer_move(int player_move)
{
    int best_move = 0, i, best_score = -100, score;
    float chance = 0.0, best_chance = -1.0;

    switch (difficulty)
    {
    /* Easy mode, randomm computer move */
    case 1:
        int choice, success = 0;
        do
        {
            i = rand() % 8 + 1;                       // random number from 0 to 8
            if (square[i] != 'X' && square[i] != 'O') // if the random number choice is not occupied, exit while loop
                success++;
        } while (success == 0);
        best_move = i;

        break;

    /* Medium mode, naive bayes */
    case 2:
        /*change mark to lowercase to match with dataset*/
        char temp_player_mark = PLAYER_MARK + 32;
        char temp_com_mark = COM_MARK + 32;

        /* If computer start first, random move */
        if (player_move == -1)
        {
            int choice, success = 0;
            do
            {
                i = rand() % 8 + 1;                       // random number from 0 to 8
                if (square[i] != 'X' && square[i] != 'O') // if the random number choice is not occupied, exit while loop
                {
                    success++;
                }
            } while (success == 0);
            best_move = i;

            break;
        }
        else
        {
            // calculate probability of computer winning based on player's move
            neg_posterior = calc_probability(temp_player_mark, player_move, 1);

            // For every empty square, calculate probability for computer to win, highest probability will be the best move
            for (i = 0; i < 9; i++)
            {
                if (square[i] != 'X' && square[i] != 'O')
                {
                    chance = calc_probability(temp_com_mark, i, 0);
                    if (chance > best_chance)
                    {
                        best_chance = chance;
                        best_move = i;
                    }
                }
            }
        }
        // update probability
        best_chance = calc_probability(temp_com_mark, best_move, 1);
        break;

    /* Hard mode, minimax, not winnable*/
    case 3:
        for (i = 0; i < 9; i++)
        {
            if (square[i] != 'X' && square[i] != 'O') // if space is available
            {
                square[i] = COM_MARK;  // temporarily place com_mark in square array index
                score = minimax(0, 0); // minimise, depth 0
                square[i] = i + '1';   // change square index back to the number (eg index 2 to '2')
                if (score > best_score)
                {
                    best_score = score;
                    best_move = i;
                }
            }
        }
        break;
    default:
        break;
    }

    square[best_move] = COM_MARK; // update square array

    GtkWidget *image = gtk_image_new_from_file("resources/O.png");
    gtk_widget_set_parent(image, square_btn[best_move]); // change button label to computer mark
}

/* To draw grid lines */
static void draw_function(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer data)
{
    cairo_set_source_rgb(cr, 0, 0, 0); // Line colour
    cairo_set_line_width(cr, 4);       // Line width

    cairo_move_to(cr, 170, 0);
    cairo_line_to(cr, 170, 520);

    cairo_move_to(cr, 350, 0);
    cairo_line_to(cr, 350, 520);

    cairo_move_to(cr, 0, 170);
    cairo_line_to(cr, 520, 170);

    cairo_move_to(cr, 0, 350);
    cairo_line_to(cr, 520, 350);

    cairo_stroke(cr); // draw the lines
}

/* Common gamescreen to be used by both single player and two player mode */
/* Label for x and o, play again and main menu buttons, and 9 buttons to play tic tac toe */
void gamescreen()
{
    int x = 100, y = 160, i = 0;

    clear_layout();

    backgnd = gtk_image_new_from_file("resources/game-backgnd.jpg");
    gtk_widget_set_size_request(backgnd, 720, 720);
    gtk_fixed_put(GTK_FIXED(layout), backgnd, 0, 0);

    /* draw grid lines */
    GtkWidget *area = gtk_drawing_area_new();
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(area), 520);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(area), 520);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area), draw_function, NULL, NULL);

    gtk_fixed_put(GTK_FIXED(layout), area, 100, 160);

    x_label = gtk_label_new("");
    o_label = gtk_label_new("");
    left_score_label = gtk_label_new("");
    right_score_label = gtk_label_new("");

    gtk_label_set_markup(GTK_LABEL(left_score_label), g_strdup_printf("<span size = 'x-large' foreground = 'grey'>%d</span>", player1_score));
    gtk_label_set_markup(GTK_LABEL(right_score_label), g_strdup_printf("<span size = 'x-large' foreground = 'grey'>%d</span>", player2_score));

    if (current_player == 3)
    {
        // left and right no difference
        gtk_label_set_markup(GTK_LABEL(x_label), "<span size = 'x-large'><u>Player 1 (X)</u></span>");
        gtk_label_set_markup(GTK_LABEL(o_label), "<span size = 'x-large'><u>Player 2 (O)</u></span>");
    }
    else
    {
        // bold left
        gtk_label_set_markup(GTK_LABEL(x_label), "<span size = 'x-large'><u><b>Player 1 (X)</b></u></span>");

        // grey right
        gtk_label_set_markup(GTK_LABEL(o_label), "<span size = 'x-large' foreground = 'grey'><u>Player 2 (O)</u></span>");
    }

    tie_label = gtk_label_new("");
    gtk_label_set_markup(GTK_LABEL(tie_label), "<span size = 'x-large' foreground = 'grey'>Tie</span>");

    tie_score_label = gtk_label_new("");
    gtk_label_set_markup(GTK_LABEL(tie_score_label), g_strdup_printf("<span size = 'x-large' foreground = 'grey'>%d</span>", tie_score));

    /* play again button and main menu button */
    replay_btn = gtk_button_new_with_label("");
    g_signal_connect(replay_btn, "clicked", G_CALLBACK(restart), NULL);
    gtk_label_set_markup(GTK_LABEL(gtk_widget_get_first_child(replay_btn)), "<span size = 'large'>Restart</span>");
    placeWidget(replay_btn, 200, 100, 140, 40);

    button = gtk_button_new_with_label("");
    g_signal_connect(button, "clicked", G_CALLBACK(main_menu), NULL);
    gtk_label_set_markup(GTK_LABEL(gtk_widget_get_first_child(button)), "<span size = 'large'>Main Menu</span>");
    placeWidget(button, 380, 100, 140, 40);

    /* Align labels and attach to layout*/
    placeLabel(x_label, 80, 40, 100, 20, 0.5, GTK_ALIGN_END);
    placeLabel(o_label, 540, 40, 100, 20, 0.5, GTK_ALIGN_END);
    placeLabel(tie_label, 320, 60, 80, 20, 0.5, GTK_ALIGN_CENTER);
    placeLabel(left_label, 80, 60, 100, 20, 0.5, GTK_ALIGN_CENTER);
    placeLabel(right_label, 540, 60, 100, 20, 0.5, GTK_ALIGN_CENTER);
    placeLabel(tie_score_label, 320, 80, 80, 20, 0.5, GTK_ALIGN_START);
    placeLabel(left_score_label, 80, 80, 100, 20, 0.5, GTK_ALIGN_START);
    placeLabel(right_score_label, 540, 80, 100, 20, 0.5, GTK_ALIGN_START);

    /* nine buttons for tic tac toe game */
    for (i = 0; i < 9; i++)
    {
        square_btn[i] = gtk_button_new();
        gtk_widget_set_sensitive(square_btn[i], TRUE);

        g_signal_connect(square_btn[i], "clicked", G_CALLBACK(square_clicked), NULL);

        placeWidget(square_btn[i], x, y, 160, 160);

        /*             positions for buttons           */
        /*  |   100, 160  |   280,160  |   460,160  |  */
        /*  |   100, 340  |   280,340  |   460,340  |  */
        /*  |   100, 420  |   280,420  |   460,420  |  */

        if (x == 460)
        {
            x = 100;
            y += 180;
        }
        else
            x += 180;
    }
}

void winning_line(char winner)
{
    checkrow(); // get winning row values
    GtkWidget *image;
    if (winner == 'X')
    {
        for (int i = 0; i < 3; i++)
        {
            image = gtk_image_new_from_file("resources/X_green.png");
            gtk_widget_unparent(gtk_widget_get_last_child(square_btn[winning_row[i]]));
            gtk_widget_set_parent(image, square_btn[winning_row[i]]);
        }
    }
    else if (winner == 'O')
    {
        for (int i = 0; i < 3; i++)
        {
            image = gtk_image_new_from_file("resources/O_green.png");
            gtk_widget_unparent(gtk_widget_get_last_child(square_btn[winning_row[i]]));
            gtk_widget_set_parent(image, square_btn[winning_row[i]]);
        }
    }
}

/* Function to be called back when a square is clicked in game */
static void square_clicked(GtkWidget *widget, gpointer data)
{
    int index, k;

    /*get index of button pressed in square_btn array*/
    for (int i = 0; i < 9; i++)
    {
        if (square_btn[i] == widget)
        {
            index = i;
            break;
        }
    }

    if (square[index] != 'X' && square[index] != 'O') // empty square
    {
        GtkWidget *image;
        /* Two player */
        if (current_player == 1) // player 1
        {
            // update array
            square[index] = 'X';

            // mark square btn
            image = gtk_image_new_from_file("resources/X.png");
            gtk_widget_set_parent(image, widget);

            k = checkwin(); // check if game has ended
            if (k == 1)     // game ended, win
            {
                winning_line('X');

                if (*player1_name == '\0') // if player name is empty, use default name
                    showdialog("We have a winner!", "Player 1 win!");
                else
                    showdialog("We have a winner!", g_strdup_printf("%s win!", player1_name));

                // update score
                player1_score++;
                end_game(k);
            }
            else if (k == 2)
                end_game(k);
            else
            {
                // game cont, change to player 2
                // grey player 1
                gtk_label_set_markup(GTK_LABEL(x_label), "<span size = 'x-large' foreground = 'grey'><u>Player 1 (X)</u></span>");
                gtk_label_set_markup(GTK_LABEL(left_label), g_strdup_printf("<span size = 'x-large' foreground = 'grey'>%s</span>", player1_name));

                // bold player 2
                gtk_label_set_markup(GTK_LABEL(o_label), "<span size = 'x-large'><u><b>Player 2 (O)</b></u></span>");
                gtk_label_set_markup(GTK_LABEL(right_label), g_strdup_printf("<span size = 'x-large'><b>%s</b></span>", player2_name));

                current_player++;
            }
        }
        else if (current_player == 2) // player 2
        {
            // update array
            square[index] = 'O';

            // mark square btn
            image = gtk_image_new_from_file("resources/O.png");
            gtk_widget_set_parent(image, widget);

            k = checkwin(); // check if game has ended
            if (k == 1)     // game ended, win
            {
                winning_line('O');

                if (*player2_name == '\0') // if player name is empty, use default name
                    showdialog("We have a winner!", "Player 2 win!");
                else
                    showdialog("We have a winner!", g_strdup_printf("%s win!", player2_name));

                // update score
                player2_score++;

                end_game(k);
            }
            else if (k == 2)
                end_game(k);
            else
            {
                // game cont, change to player 1
                // grey player 2
                gtk_label_set_markup(GTK_LABEL(o_label), "<span size = 'x-large' foreground = 'grey'><u>Player 2 (O)</u></span>");
                gtk_label_set_markup(GTK_LABEL(right_label), g_strdup_printf("<span size = 'x-large' foreground = 'grey'>%s</span>", player2_name));

                // bold player 1
                gtk_label_set_markup(GTK_LABEL(x_label), "<span size = 'x-large'><u><b>Player 1 (X)</b></u></span>");
                gtk_label_set_markup(GTK_LABEL(left_label), g_strdup_printf("<span size = 'x-large'><b>%s</b></span>", player1_name));

                current_player--;
            }
        }

        /* Single player */
        else if (current_player == 3)
        {
            // update array
            square[index] = 'X';

            // mark square btn
            image = gtk_image_new_from_file("resources/X.png");
            gtk_widget_set_parent(image, widget);

            k = checkwin(); // check if game has ended
            if (k == 1)     // game ended, win
            {
                winning_line('X');

                // bold player label
                gtk_label_set_markup(GTK_LABEL(x_label), "<span size = 'x-large'><u><b>Player 1 (X)</b></u></span>");
                gtk_label_set_markup(GTK_LABEL(left_label), "<span size = 'x-large'><b>You</b></span>");

                // grey computer label
                gtk_label_set_markup(GTK_LABEL(o_label), "<span size = 'x-large' foreground = 'grey'><u>Player 2 (O)</u></span>");
                gtk_label_set_markup(GTK_LABEL(right_label), "<span size = 'x-large' foreground = 'grey'>Computer</span>");

                showdialog("We have a winner!", "You win!");

                // update score and label
                player1_score++;

                end_game(k);
            }
            else if (k == 2) // game ended, draw
                end_game(k);
            else // game continue
            {
                computer_move(index);
                k = checkwin(); // check if game has ended after computer move
                if (k == 1)     // win
                {
                    winning_line('O');

                    // bold computer label
                    gtk_label_set_markup(GTK_LABEL(o_label), "<span size = 'x-large'><u><b>Player 2 (O)</b></u></span>");
                    gtk_label_set_markup(GTK_LABEL(right_label), "<span size = 'x-large'><b>Computer</b></span>");

                    // grey player label
                    gtk_label_set_markup(GTK_LABEL(x_label), "<span size = 'x-large' foreground = 'grey'><u>Player 1 (X)</u></span>");
                    gtk_label_set_markup(GTK_LABEL(left_label), "<span size = 'x-large' foreground = 'grey'>You</span>");

                    showdialog("We have a winner!", "Computer win!");

                    // update score and label
                    player2_score++;
                    end_game(k);
                }
                else if (k == 2) // draw
                    end_game(k);
            }
        }
    }
    else // square is not empty, show dialog
        showdialog("Invalid", "The square is in use!");
}

/* When game ends, set all square buttons to inactive, set replay button to be play again*/
void end_game(int result)
{
    if (result == 2) // if tie
    {
        showdialog("Game draw", "It's a tie!");
        tie_score++;
        // bold tie label
        gtk_label_set_markup(GTK_LABEL(tie_label), "<span size = 'x-large'><b>Tie</b></span>");

        // grey players
        gtk_label_set_markup(GTK_LABEL(o_label), "<span size = 'x-large' foreground = 'grey'><u>Player 2 (O)</u></span>");
        gtk_label_set_markup(GTK_LABEL(x_label), "<span size = 'x-large' foreground = 'grey'><u>Player 1 (X)</u></span>");

        if (current_player == 3)
        {
            gtk_label_set_markup(GTK_LABEL(left_label), "<span size = 'x-large' foreground = 'grey'>You</span>");
            gtk_label_set_markup(GTK_LABEL(right_label), g_strdup_printf("<span size = 'x-large' foreground = 'grey'>Computer</span>"));
        }
        else
        {
            gtk_label_set_markup(GTK_LABEL(left_label), g_strdup_printf("<span size = 'x-large' foreground = 'grey'>%s</span>", player1_name));
            gtk_label_set_markup(GTK_LABEL(right_label), g_strdup_printf("<span size = 'x-large' foreground = 'grey'>%s</span>", player2_name));
        }
    }

    gtk_label_set_markup(GTK_LABEL(left_score_label), g_strdup_printf("<span size = 'x-large'><b>%d</b></span>", player1_score));
    gtk_label_set_markup(GTK_LABEL(tie_score_label), g_strdup_printf("<span size = 'x-large'><b>%d</b></span>", tie_score));
    gtk_label_set_markup(GTK_LABEL(right_score_label), g_strdup_printf("<span size = 'x-large'><b>%d</b></span>", player2_score));

    for (int i = 0; i < 9; i++)
        gtk_widget_set_sensitive(square_btn[i], FALSE);

    g_signal_connect(replay_btn, "clicked", G_CALLBACK(replay), NULL);
    g_signal_handlers_disconnect_by_func(replay_btn, restart, NULL);
    gtk_label_set_markup(GTK_LABEL(gtk_widget_get_first_child(replay_btn)), "<span size = 'large'>Play Again</span>");
}

/* Function be called back when play again button is presed */
/* reinitialise square array and buttons */
static void replay(GtkWidget *widget, gpointer data)
{
    /* Reinitialise square array */
    for (int i = 0; i < 9; i++)
    {
        gtk_widget_set_sensitive(square_btn[i], TRUE);
        if (square[i] == 'X' || square[i] == 'O')
        {
            gtk_widget_unparent(gtk_widget_get_last_child(square_btn[i]));
            square[i] = i + '1';
        }
    }

    // grey score labels
    gtk_label_set_markup(GTK_LABEL(left_score_label), g_strdup_printf("<span size = 'x-large' foreground = 'grey'>%d</span>", player1_score));
    gtk_label_set_markup(GTK_LABEL(right_score_label), g_strdup_printf("<span size = 'x-large' foreground = 'grey'>%d</span>", player2_score));
    gtk_label_set_markup(GTK_LABEL(tie_label), "<span size = 'x-large' foreground = 'grey'>Tie</span>");
    gtk_label_set_markup(GTK_LABEL(tie_score_label), g_strdup_printf("<span size = 'x-large' foreground = 'grey'>%d</span>", tie_score));

    if (current_player == 3)
    {
        if (difficulty == 2)
        {
            neg_posterior = total_negative / TOTAL;
            pos_posterior = total_positive / TOTAL;
        }
        // player
        gtk_label_set_markup(GTK_LABEL(x_label), "<span size = 'x-large'><u>Player 1 (X)</u></span>");
        gtk_label_set_markup(GTK_LABEL(left_label), "<span size = 'x-large'>You</span>");

        // computer
        gtk_label_set_markup(GTK_LABEL(o_label), "<span size = 'x-large'><u>Player 2 (O)</u></span>");
        gtk_label_set_markup(GTK_LABEL(right_label), "<span size = 'x-large'>Computer</span>");
    }

    /* Set play again button back to restart */
    g_signal_connect(replay_btn, "clicked", G_CALLBACK(restart), NULL);
    g_signal_handlers_disconnect_by_func(replay_btn, replay, NULL);
    gtk_label_set_markup(GTK_LABEL(gtk_widget_get_first_child(replay_btn)), "<span size = 'large'>Restart</span>");

    if (starting_player == 1)
    {
        /*if starting player is 1, change to 2 and vice versa*/
        starting_player++;

        /* if single player and starting player is 1, change to computer start first*/
        if (current_player == 3)
            computer_move(-1);

        else
        {
            current_player = 2;
            // grey player 1
            gtk_label_set_markup(GTK_LABEL(x_label), "<span size = 'x-large' foreground = 'grey'><u>Player 1 (X)</u></span>");
            gtk_label_set_markup(GTK_LABEL(left_label), g_strdup_printf("<span size = 'x-large' foreground = 'grey'>%s</span>", player1_name));

            // bold player 2
            gtk_label_set_markup(GTK_LABEL(o_label), "<span size = 'x-large'><u><b>Player 2 (O)</b></u></span>");
            gtk_label_set_markup(GTK_LABEL(right_label), g_strdup_printf("<span size = 'x-large'><b>%s</b></span>", player2_name));
        }
    }
    else
    {
        starting_player--;
        if (current_player != 3)
        {
            // grey player 2
            gtk_label_set_markup(GTK_LABEL(o_label), "<span size = 'x-large' foreground = 'grey'><u>Player 2 (O)</u></span>");
            gtk_label_set_markup(GTK_LABEL(right_label), g_strdup_printf("<span size = 'x-large' foreground = 'grey'>%s</span>", player2_name));

            // bold player 1
            gtk_label_set_markup(GTK_LABEL(x_label), "<span size = 'x-large'><u><b>Player 1 (X)</b></u></span>");
            gtk_label_set_markup(GTK_LABEL(left_label), g_strdup_printf("<span size = 'x-large'><b>%s</b></span>", player1_name));

            current_player = 1;
        }
    }
}

/* Function be called back when restart button is presed */
static void restart(GtkWidget *widget, gpointer data)
{
    /* Reinitialise square array */
    for (int i = 0; i < 9; i++)
    {
        gtk_widget_set_sensitive(square_btn[i], TRUE);
        if (square[i] == 'X' || square[i] == 'O')
        {
            gtk_widget_unparent(gtk_widget_get_last_child(square_btn[i]));
            square[i] = i + '1';
        }
    }

    if (current_player == 3 && difficulty == 2)
    {
        neg_posterior = total_negative / TOTAL;
        pos_posterior = total_positive / TOTAL;
    }

    /* if starting player is 1, player 1 start again*/
    if (starting_player == 1)
    {
        /* if two player mode, current player set to player 1*/
        if (current_player != 3)
        {
            current_player = 1;
            // grey player 2
            gtk_label_set_markup(GTK_LABEL(o_label), "<span size = 'x-large' foreground = 'grey'><u>Player 2 (O)</u></span>");
            gtk_label_set_markup(GTK_LABEL(right_label), g_strdup_printf("<span size = 'x-large' foreground = 'grey'>%s</span>", player2_name));

            // bold player 1
            gtk_label_set_markup(GTK_LABEL(x_label), "<span size = 'x-large'><u><b>Player 1 (X)</b></u></span>");
            gtk_label_set_markup(GTK_LABEL(left_label), g_strdup_printf("<span size = 'x-large'><b>%s</b></span>", player1_name));
        }
    }
    else
    {
        /* if starting player is 2, computer start first move */
        if (current_player == 3)
            computer_move(-1);
        else
        /* if two player mode and starting player is 2, set current player to 2*/
        {
            // grey player 1
            gtk_label_set_markup(GTK_LABEL(x_label), "<span size = 'x-large' foreground = 'grey'><u>Player 1 (X)</u></span>");
            gtk_label_set_markup(GTK_LABEL(left_label), g_strdup_printf("<span size = 'x-large' foreground = 'grey'>%s</span>", player1_name));

            // bold player 2
            gtk_label_set_markup(GTK_LABEL(o_label), "<span size = 'x-large'><u><b>Player 2 (O)</b></u></span>");
            gtk_label_set_markup(GTK_LABEL(right_label), g_strdup_printf("<span size = 'x-large'><b>%s</b></span>", player2_name));
            current_player = 2;
        }
    }
}

void main_layout()
{
    /*backgnd image*/
    backgnd = gtk_image_new_from_file("resources/main-backgnd.png");
    gtk_widget_set_size_request(backgnd, 720, 720);
    gtk_fixed_put(GTK_FIXED(layout), backgnd, 0, 0);

    // window title
    gtk_window_set_title(GTK_WINDOW(main_window), "Tic Tac Toe");

    /* buttons */
    button = gtk_button_new_with_label("");
    g_signal_connect(button, "clicked", G_CALLBACK(single_player), NULL);
    gtk_label_set_markup(GTK_LABEL(gtk_widget_get_first_child(button)), "<span size = 'large' foreground = 'green'><b>Single Player</b></span>");
    placeWidget(button, 280, 200, BUTTON_WIDTH, BUTTON_HEIGHT);

    button = gtk_button_new_with_label("");
    g_signal_connect(button, "clicked", G_CALLBACK(two_player), NULL);
    gtk_label_set_markup(GTK_LABEL(gtk_widget_get_first_child(button)), "<span size = 'large' foreground = 'orange'><b>Two Player</b></span>");
    placeWidget(button, 280, 270, BUTTON_WIDTH, BUTTON_HEIGHT);

    button = gtk_button_new_with_label("");
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), main_window);
    gtk_label_set_markup(GTK_LABEL(gtk_widget_get_first_child(button)), "<span size = 'large' foreground = 'red'><b>Quit</b></span>");
    placeWidget(button, 280, 340, BUTTON_WIDTH, BUTTON_HEIGHT);

    // initialise values
    player1_score = 0;
    player2_score = 0;
    tie_score = 0;

    neg_posterior = total_negative / TOTAL;
    pos_posterior = total_positive / TOTAL;
}

/* main menu, allow player to choose between single player and two player mode */
static void main_menu(GtkWidget *widget, gpointer data)
{
    /* reinitialise square array */
    for (int i = 0; i < 9; i++)
    {
        if (square[i] == 'X' || square[i] == 'O')
        {
            gtk_widget_unparent(gtk_widget_get_last_child(square_btn[i]));
            square[i] = i + '1';
        }
    }

    clear_layout();

    main_layout();
}

/* First main menu screen when user open app */
/* Initialise window and grid */
static void setup(GtkApplication *app, gpointer user_data)
{
    // init window
    main_window = gtk_application_window_new(app);

    // initialise values
    for (int i = 0; i < 9; i++)
    {
        square[i] = i + '1';
    }

    // set window size and turn resizable off
    gtk_window_set_default_size(GTK_WINDOW(main_window), 720, 720);
    gtk_window_set_resizable(GTK_WINDOW(main_window), FALSE);

    // init layout
    layout = gtk_fixed_new();
    gtk_window_set_child(GTK_WINDOW(main_window), layout);

    main_layout();

    gtk_widget_show(main_window);
}

int main(int argc, char **argv)
{
    get_data();

    test_data();
    GtkApplication *app;

    int status;

    app = gtk_application_new("org.tic-tac-toe", G_APPLICATION_FLAGS_NONE);

    g_signal_connect(app, "activate", G_CALLBACK(setup), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
