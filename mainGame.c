#include <gtk/gtk.h>
#include "ai.h"
#include "ttt.h"
#include "gtk_widgets.h"
#include "minimax.h"

static void test_data(GtkWidget *widget, gpointer data);

static void easy(GtkWidget *widget, gpointer data);
static void medium(GtkWidget *widget, gpointer data);
static void hard(GtkWidget *widget, gpointer data);

static void two_player(GtkWidget *widget, gpointer data);
static void two_player_gamescreen(GtkWidget *widget, gpointer data);
static void singleplayer(GtkWidget *widget, gpointer data);
void singleplayer_gamescreen();
void computer_move();
void gamescreen();

static void main_menu(GtkWidget *widget, gpointer data);
static void restart(GtkWidget *widget, gpointer data);
static void square_clicked(GtkWidget *widget, gpointer data);
static void replay(GtkWidget *widget, gpointer data);
static void setup(GtkApplication *app, gpointer user_data);
int main();

static void test_data(GtkWidget *widget, gpointer data)
{
    double positive_prior = total_positive / 958.0;
    double negative_prior = total_negative / 958.0;

    int positive_testing = total_positive * 0.2; // number of test positive
    int negative_testing = total_negative * 0.2; // number of test negative
    int tp = 0, tn = 0, fp = 0, fn = 0, k = 0;   // values for confusion matrix

    int positive = total_positive * 0.8;

    float error = 0, total_testing = positive_testing + negative_testing;
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
                p_probability = p_probability * x_trained[k + 9];
                n_probability = n_probability * x_trained[k];
            }

            else if (dataset[i + positive][k] == 'o')
            {
                p_probability = p_probability * o_trained[k + 9];
                n_probability = n_probability * o_trained[k];
            }
            else if (dataset[i + positive][k] == 'b')
            {
                p_probability = p_probability * b_trained[k + 9];
                n_probability = n_probability * b_trained[k];
            }
        }

        if (p_probability > n_probability) // True positive
            tp++;

        else
        {
            error++; // increase count of error output
            fn++;    // False Negative
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
                p_probability = p_probability * x_trained[k + 9];
                n_probability = n_probability * x_trained[k];
            }

            else if (dataset[i][k] == 'o')
            {
                p_probability = p_probability * o_trained[k + 9];
                n_probability = n_probability * o_trained[k];
            }
            else if (dataset[i][k] == 'b')
            {
                p_probability = p_probability * b_trained[k + 9];
                n_probability = n_probability * b_trained[k];
            }
        }

        if (p_probability > n_probability)
        {
            error++; // increase count of error
            fp++;    // False Positive
        }

        else
            tn++; // True Negative
    }

    gtk_window_set_title(GTK_WINDOW(main_window), "Test Data");
    gtk_grid_set_column_spacing(GTK_GRID(grid), 0);
    /*clear previous widgets*/
    GtkWidget *childs = gtk_widget_get_first_child(grid);
    while (childs != NULL)
    {
        gtk_grid_remove(GTK_GRID(grid), childs);
        childs = gtk_widget_get_first_child(grid);
    }

    label = gtk_label_new("Total tested");
    gtk_widget_set_size_request(label, 100, 20);
    gtk_grid_attach(GTK_GRID(grid), space, 0, 0, 3, 1);

    /*print results
    printf("\n                    Total tested: %1.0f", (total_testing));
    printf("\n---------------------------------------------------------");
    printf("\n|      True Positive (TP)    |    False Positive (FP)     |");
    printf("\n|              %d           |             %d             |", tp, fp);
    printf("\n|---------------------------------------------------------|");
    printf("\n|      False Negative (FN)   |    True Negative (TN)      |");
    printf("\n|              %d            |             %d             |", fn, tn);
    printf("\n---------------------------------------------------------");
    printf("\nTotal number of error: %1.0f", error);
    printf("\nAccuracy: %0.1f%% \n\n", (((total_testing - error) / total_testing) * 100));
    */
}

static void easy(GtkWidget *widget, gpointer data)
{
    difficulty = 1;
    singleplayer_gamescreen();
}

static void medium(GtkWidget *widget, gpointer data)
{
    difficulty = 2;
    singleplayer_gamescreen();
}

static void hard(GtkWidget *widget, gpointer data)
{
    difficulty = 3;
    singleplayer_gamescreen();
}

/* Set up widgets for two player game */
/* Change window title, let users key in names and set up start button to start game */
static void two_player(GtkWidget *widget, gpointer data)
{
    gtk_window_set_title(GTK_WINDOW(main_window), "Two Player"); // change window title
    clear_grid();

    /* space on the left of screen */
    space = gtk_label_new("");
    gtk_widget_set_size_request(space, 280, 560);
    gtk_grid_attach(GTK_GRID(grid), space, 0, 0, 14, 28);

    /* space on right of screen */
    space = gtk_label_new("");
    gtk_widget_set_size_request(space, 280, 720);
    gtk_grid_attach(GTK_GRID(grid), space, 22, 0, 14, 36);

    /* Entries for players to key in names for labels in the game screen */
    player1 = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(player1), "Player 1");
    gtk_grid_attach(GTK_GRID(grid), player1, 14, 10, 8, 1);

    player2 = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(player2), "Player 2");
    gtk_grid_attach(GTK_GRID(grid), player2, 14, 12, 8, 1);

    /* Start button */
    button = gtk_button_new_with_label("Start");
    g_signal_connect(button, "clicked", G_CALLBACK(two_player_gamescreen), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 14, 15, 8, 1);

    /* Back button to go back to main menu */
    button = gtk_button_new_with_label("Back");
    g_signal_connect(button, "clicked", G_CALLBACK(main_menu), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 2, 29, 4, 1);
}

/* initialise left and right labels (cannot attach yet as gamescreen function will remove all previous widgets) */
static void two_player_gamescreen(GtkWidget *widget, gpointer data)
{
    GtkWidget *player1_label, *player2_label;

    int x = 3, y = 3, i = 0;

    current_player = 1;
    starting_player = 1;
    /* Get player nammes from previous screen and set label names */
    player1_name = g_strdup(gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(player1))));
    player2_name = g_strdup(gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(player2))));

    left_label = gtk_label_new(player1_name);
    right_label = gtk_label_new(player2_name);

    gamescreen(); // standard gamescreen
}

/* Screen where user chooses difficulty before starting single player game */
/* Change window title, insert three buttons for three difficulty and include a back button*/
static void single_player(GtkWidget *widget, gpointer data)
{
    gtk_window_set_title(GTK_WINDOW(main_window), "Single Player"); // change window title

    clear_grid();

    /* space on the left of screen */
    space = gtk_label_new("");
    gtk_widget_set_size_request(space, 280, 560);
    gtk_grid_attach(GTK_GRID(grid), space, 0, 0, 14, 28);

    /* space on right of screen */
    space = gtk_label_new("");
    gtk_widget_set_size_request(space, 280, 720);
    gtk_grid_attach(GTK_GRID(grid), space, 22, 0, 14, 36);

    label = gtk_label_new("Choose your difficulty");
    gtk_widget_set_size_request(label, 160, 20);
    gtk_grid_attach(GTK_GRID(grid), label, 14, 8, 8, 1);

    /* Difficulty buttons, each button links to different callback functions to change global variable 'difficulty' to change how computer plays */
    button = gtk_button_new_with_label("Easy");
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(easy), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 14, 12, 8, 1);

    button = gtk_button_new_with_label("Medium");
    g_signal_connect(button, "clicked", G_CALLBACK(medium), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 14, 14, 8, 1);

    button = gtk_button_new_with_label("Hard");
    g_signal_connect(button, "clicked", G_CALLBACK(hard), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 14, 16, 8, 1);

    /* Back button to go back to main menu */
    button = gtk_button_new_with_label("Back");
    g_signal_connect(button, "clicked", G_CALLBACK(main_menu), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 2, 29, 4, 1);
}

/* initialise left and right labels (cannot attach yet as gamescreen function will remove all previous widgets) */
void singleplayer_gamescreen()
{
    current_player = 3;
    starting_player = 1;
    left_label = gtk_label_new("You");       // x
    right_label = gtk_label_new("Computer"); // o

    gamescreen();
}

/* Called after every player move and if game is continuing */
void computer_move(int player_move)
{
    char com_mark = 'O';
    int best_move = 0, i, best_score = -20, score;
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
            {
                success++;
            }
        } while (success == 0);
        best_move = i;

        break;

    /* Medium mode, naive bayes */
    case 2:
        /*change mark to lowercase to match with dataset*/

        mark += 32;
        com_mark += 32;

        // if player not equal to -1, means player made a move, calculate probability of computer winning based on player's move
        if (player_move != -1)
            probability = calc_probability(mark, player_move);

        // For every empty square, calculate probability for computer to win, highest probability will be the best move
        for (int i = 0; i < 9; i++)
        {
            if (square[i] != 'X' && square[i] != 'O')
            {
                chance = calc_probability(com_mark, i);
                if (chance > best_chance)
                {
                    best_chance = chance;
                    best_move = i;
                }
            }
        }

        // update probability
        probability = best_chance;

        /* change mark back to uppercase*/
        mark -= 32;
        com_mark -= 32;
        break;

    /* Hard mode, minimax, not winnable*/
    case 3:

        for (i = 0; i < 9; i++)
        {
            if (square[i] != 'X' && square[i] != 'O') // if space is available
            {
                square[i] = com_mark;         // temporarily place com_mark in square array index
                score = minimax(0, com_mark); // minimise
                square[i] = i + '1';          // change square index back to the number (ie index 2 to '2')
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
    square[best_move] = com_mark;                                 // update square array
    gtk_button_set_label(GTK_BUTTON(square_btn[best_move]), "O"); // change button label to computer mark
}

/* When game ends, set all square buttons to inactive, set play again button to be active*/
void end_game()
{
    for (int i = 0; i < 9; i++)
    {
        gtk_widget_set_sensitive(square_btn[i], FALSE);
    }

    g_signal_handlers_disconnect_by_func(replay_btn, restart, NULL);
    gtk_button_set_label(GTK_BUTTON(replay_btn), "Play Again");
    g_signal_connect(replay_btn, "clicked", G_CALLBACK(replay), NULL);
}

/* Common gamescreen to be used by both single player and two player mode */
/* Label for x and o, play again and main menu buttons, and 9 buttons to play tic tac toe */
void gamescreen()
{
    int x = 5, y = 7, i = 0;

    clear_grid();

    x_label = gtk_label_new("(X)");
    gtk_label_set_markup(GTK_LABEL(x_label), "<b>(X)</b>"); // player 1 start, bold x label
    o_label = gtk_label_new("(O)");

    right_score_label = gtk_label_new(g_strdup_printf("Score - %d", player1_score));
    left_score_label = gtk_label_new(g_strdup_printf("Score - %d", player2_score));
    /* space on the left */
    space = gtk_label_new("");

    gtk_widget_set_size_request(space, 80, 720);
    gtk_grid_attach(GTK_GRID(grid), space, 0, 0, 4, 36);

    /* space on the right */
    space = gtk_label_new("");
    gtk_widget_set_size_request(space, 80, 720);
    gtk_grid_attach(GTK_GRID(grid), space, 32, 0, 4, 36);

    /* play again button and main menu button */
    replay_btn = gtk_button_new_with_label("Restart");
    g_signal_connect(replay_btn, "clicked", G_CALLBACK(restart), NULL);
    gtk_grid_attach(GTK_GRID(grid), replay_btn, 10, 4, 7, 1);

    main_menu_btn = gtk_button_new_with_label("Main Menu");
    g_signal_connect(main_menu_btn, "clicked", G_CALLBACK(main_menu), NULL);
    gtk_grid_attach(GTK_GRID(grid), main_menu_btn, 19, 4, 7, 1);

    /* Align labels */

    gtk_widget_set_size_request(left_label, 160, 20);
    gtk_widget_set_size_request(right_label, 160, 20);

    gtk_label_set_xalign(GTK_LABEL(x_label), 0.0);
    gtk_widget_set_valign(x_label, GTK_ALIGN_START);
    gtk_label_set_xalign(GTK_LABEL(o_label), 1.0);
    gtk_widget_set_valign(o_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), x_label, 4, 4, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), o_label, 29, 4, 3, 1);

    gtk_label_set_xalign(GTK_LABEL(left_label), 0.0);
    gtk_widget_set_valign(left_label, GTK_ALIGN_END);
    gtk_label_set_xalign(GTK_LABEL(right_label), 1.0);
    gtk_widget_set_valign(right_label, GTK_ALIGN_END);
    gtk_grid_attach(GTK_GRID(grid), left_label, 4, 3, 8, 1);
    gtk_grid_attach(GTK_GRID(grid), right_label, 24, 3, 8, 1);

    gtk_label_set_xalign(GTK_LABEL(left_score_label), 0.0);
    gtk_widget_set_valign(left_score_label, GTK_ALIGN_START);
    gtk_label_set_xalign(GTK_LABEL(right_score_label), 1.0);
    gtk_widget_set_valign(right_score_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), left_score_label, 4, 5, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), right_score_label, 29, 5, 3, 1);

    /* nine buttons for tic tac toe game */
    for (i = 0; i < 9; i++)
    {
        square_btn[i] = gtk_button_new();
        gtk_widget_set_sensitive(square_btn[i], TRUE);
        gtk_widget_set_size_request(square_btn[i], 160, 160);
        g_signal_connect(square_btn[i], "clicked", G_CALLBACK(square_clicked), NULL);

        gtk_grid_attach(GTK_GRID(grid), square_btn[i], x, y, 8, 8);

        /*      Grid positions for buttons      */
        /*  |   6, 8   |   14,8   |   22,8   |  */
        /*  |   6, 16  |   14,16  |   22,16  |  */
        /*  |   6, 24  |   14,24  |   22,24  |  */

        if (x == 23)
        {
            x = 5;
            y += 9;
        }
        else
        {
            x += 9;
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
        /* Two player */
        if (current_player == 1) // player 1
        {
            square[index] = 'X';
            gtk_button_set_label(GTK_BUTTON(widget), "X");
            k = checkwin(); // check if game has ended
            if (k == 1)     // game ended, win
            {
                if (*player1_name == '\0')
                    showdialog("We have a winner!", "Player 1 win");
                else
                    showdialog("We have a winner!", g_strdup_printf("%s win", player1_name));
                player1_score++;
                gtk_label_set_text(GTK_LABEL(left_score_label), g_strdup_printf("Score - %d", player1_score));
                end_game();
            }
            else
            {
                gtk_label_set_markup(GTK_LABEL(x_label), "(X)");
                gtk_label_set_markup(GTK_LABEL(o_label), "<b>(O)</b>");
                current_player++;
            }
        }
        else if (current_player == 2) // player 2
        {
            square[index] = 'O';
            gtk_button_set_label(GTK_BUTTON(widget), "O");
            k = checkwin(); // check if game has ended
            if (k == 1)     // game ended, win
            {
                if (*player2_name == '\0')
                    showdialog("We have a winner!", "Player 2 win");
                else
                    showdialog("We have a winner!", g_strdup_printf("%s win", player2_name));
                player2_score++;
                gtk_label_set_text(GTK_LABEL(right_score_label), g_strdup_printf("Score - %d", player2_score));
                end_game();
            }
            else
            {
                gtk_label_set_markup(GTK_LABEL(x_label), "<b>(X)</b>");
                gtk_label_set_markup(GTK_LABEL(o_label), "(O)");
                current_player--;
            }
        }
        /* Single player */
        else if (current_player == 3)
        {
            square[index] = 'X';
            gtk_button_set_label(GTK_BUTTON(widget), "X");
            k = checkwin(); // check if game has ended
            if (k == 1)     // game ended, win
            {
                showdialog("We have a winner!", "You win");
                player1_score++;
                gtk_label_set_text(GTK_LABEL(left_score_label), g_strdup_printf("Score - %d", player1_score));
                end_game();
            }
        }

        if (k == 2) // game ended, draw
        {
            showdialog("Game draw", "It's a draw!");
            end_game();
        }
        else if (k == 0 && current_player == 3) // game continue, single player, computer move now
        {
            computer_move(index);
            k = checkwin(); // check if game has ended after computer move
            if (k == 1)     // win
            {
                showdialog("We have a winner!", "Computer win");
                player2_score++;
                gtk_label_set_text(GTK_LABEL(right_score_label), g_strdup_printf("Score - %d", player2_score));
                end_game();
            }
            else if (k == 2) // draw
            {
                showdialog("Game draw", "It's a draw!");
                end_game();
            }
        }
    }
    else // square is not empty, show dialog
        showdialog("Invalid", "The square is in use!");
}

/* Function be called back when play again button is presed */
/* Player 1 and 2 swap places, player 1 to be player 2 and vice versa */
/* reinitialise square array and buttons */
static void replay(GtkWidget *widget, gpointer data)
{
    /* Reinitialise square array */
    for (int i = 0; i < 9; i++)
    {
        gtk_widget_set_sensitive(square_btn[i], TRUE);
        gtk_button_set_label(GTK_BUTTON(square_btn[i]), " ");
        square[i] = i + '1';
    }

    /* Set play again button back to restart */
    g_signal_connect(replay_btn, "clicked", G_CALLBACK(restart), NULL);
    g_signal_handlers_disconnect_by_func(replay_btn, replay, NULL);
    gtk_button_set_label(GTK_BUTTON(replay_btn), "Restart");

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
            gtk_label_set_markup(GTK_LABEL(x_label), "(X)");
            gtk_label_set_markup(GTK_LABEL(o_label), "<b>(O)</b>");
        }
    }
    else
    {
        starting_player--;
        gtk_label_set_markup(GTK_LABEL(x_label), "<b>(X)</b>");
        gtk_label_set_markup(GTK_LABEL(o_label), "(O)");
        if (current_player != 3)
            current_player = 1;
    }
}

/* Function be called back when restart button is presed */
static void restart(GtkWidget *widget, gpointer data)
{
    /* Reinitialise square array */
    for (int i = 0; i < 9; i++)
    {
        gtk_widget_set_sensitive(square_btn[i], TRUE);
        gtk_button_set_label(GTK_BUTTON(square_btn[i]), " ");
        square[i] = i + '1';
    }

    /* if starting player is 1, player 1 start again*/
    if (starting_player == 1)
    {
        gtk_label_set_markup(GTK_LABEL(x_label), "<b>(X)</b>");
        gtk_label_set_markup(GTK_LABEL(o_label), "(O)");
        /* if two player mode, current player set to player 1*/
        if (current_player != 3)
            current_player = 1;
    }
    else
    {
        /* if starting player is 2, computer start first move */
        if (current_player == 3)
            computer_move(-1);
        else
        /* if two player mode and starting player is 2, set current player to 2*/
        {
            gtk_label_set_markup(GTK_LABEL(x_label), "(X)");
            gtk_label_set_markup(GTK_LABEL(o_label), "<b>(O)</b>");
            current_player = 2;
        }
    }
}

/* main menu, allow player to choose between single player and two player mode */
static void main_menu(GtkWidget *widget, gpointer data)
{
    gtk_window_set_title(GTK_WINDOW(main_window), "Tic Tac Toe"); // change window title

    clear_grid();

    /* reinitialise square array */
    for (int i = 0; i < 9; i++)
    {
        square[i] = i + '1';
    }

    player1_score = 0;
    player2_score = 0;

    mark = 'X'; // reinitialise player mark

    /* space on the left of screen */
    space = gtk_label_new("");
    gtk_widget_set_size_request(space, 280, 720);
    gtk_grid_attach(GTK_GRID(grid), space, 0, 0, 14, 36);

    /* space on right of screen */
    space = gtk_label_new("");
    gtk_widget_set_size_request(space, 280, 720);
    gtk_grid_attach(GTK_GRID(grid), space, 22, 0, 14, 36);

    /* buttons */
    button = gtk_button_new_with_label("Single Player");
    g_signal_connect(button, "clicked", G_CALLBACK(single_player), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 14, 10, 8, 1);

    button = gtk_button_new_with_label("Two Player");
    g_signal_connect(button, "clicked", G_CALLBACK(two_player), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 14, 12, 8, 1);

    button = gtk_button_new_with_label("Test Data");
    g_signal_connect(button, "clicked", G_CALLBACK(test_data), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 14, 14, 8, 1);

    button = gtk_button_new_with_label("Quit");
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), main_window);
    gtk_grid_attach(GTK_GRID(grid), button, 14, 16, 8, 1);
}

/* First main menu screen when user open app */
/* Initialise window and grid */
static void setup(GtkApplication *app, gpointer user_data)
{

    main_window = gtk_application_window_new(app);
    for (int i = 0; i < 9; i++)
    {
        square[i] = i + '1';
    }
    current_player = 1;
    mark = 'X';

    player1_score = 0;
    player2_score = 0;

    gtk_window_set_default_size(GTK_WINDOW(main_window), 720, 720);
    gtk_window_set_resizable(GTK_WINDOW(main_window), FALSE);

    /* construct grid */
    grid = gtk_grid_new();

    /* columns and rows same size (20) */
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);

    /* Pack the container in the window */
    gtk_window_set_child(GTK_WINDOW(main_window), grid);

    gtk_window_set_title(GTK_WINDOW(main_window), "Tic Tac Toe");

    /* space on the left of screen */
    space = gtk_label_new("");
    gtk_widget_set_size_request(space, 280, 720);
    gtk_grid_attach(GTK_GRID(grid), space, 0, 0, 14, 36);

    /* space on right of screen */
    space = gtk_label_new("");
    gtk_widget_set_size_request(space, 280, 720);
    gtk_grid_attach(GTK_GRID(grid), space, 22, 0, 14, 36);

    /* buttons */
    button = gtk_button_new_with_label("Single Player");
    g_signal_connect(button, "clicked", G_CALLBACK(single_player), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 14, 10, 8, 1);

    button = gtk_button_new_with_label("Two Player");
    g_signal_connect(button, "clicked", G_CALLBACK(two_player), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 14, 12, 8, 1);

    button = gtk_button_new_with_label("Test Data");
    g_signal_connect(button, "clicked", G_CALLBACK(test_data), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 14, 14, 8, 1);

    button = gtk_button_new_with_label("Quit");
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), main_window);
    gtk_grid_attach(GTK_GRID(grid), button, 14, 16, 8, 1);

    gtk_widget_show(main_window);
}

int main(int argc, char **argv)
{
    get_data();
    GtkApplication *app;

    int status;

    app = gtk_application_new("org.tic-tac-toe", G_APPLICATION_FLAGS_NONE);

    g_signal_connect(app, "activate", G_CALLBACK(setup), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
