#include <gtk/gtk.h>

#define TOTAL 958

GtkWidget *main_window, *grid, *button, *space, *label, *player1, *player2, *play_again_btn, *main_menu_btn, *left_label, *right_label, *x_label, *o_label;
GtkWidget *square_btn[9];

char dataset[TOTAL][10];

double probability = 1.0;

/*Total number of positive and negative outcomes*/
double total_positive = 0.0;
double total_negative = 0.0;

double x_trained[18], o_trained[18], b_trained[18]; // 0-8 for negative, 9-17 for positive
const char *player1_name;
const char *player2_name;
char mark = 'X';                                                // player mark
int difficulty;                                                 // 1 - easy, 2 - medium, 3 - hard
int current_player = 1;                                         // 1 and 2 for two player, 3 for single player
char square[9] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'}; // game state, reinitialised after every game has ended / user quit

void get_data();
void train_dataset();
void feature_possibility(char mark, int spot);
static void test_data(GtkWidget *widget, gpointer data);
double calc_probability(char input_mark, int move);
int minimax(int maximise, char com_mark);

static void easy(GtkWidget *widget, gpointer data);
static void medium(GtkWidget *widget, gpointer data);
static void hard(GtkWidget *widget, gpointer data);

static void two_player(GtkWidget *widget, gpointer data);
static void two_player_gamescreen(GtkWidget *widget, gpointer data);
static void singleplayer(GtkWidget *widget, gpointer data);
void singleplayer_gamescreen();
void computer_move();
void gamescreen();

void end_game();
static void replay(GtkWidget *widget, gpointer data);
static void main_menu(GtkWidget *widget, gpointer data);

void showdialog(const char *title, const char *message);

static void square_clicked(GtkWidget *widget, gpointer data);
static void setup(GtkApplication *app, gpointer user_data);
int checkwin();
int main();

/**** ML functions ****/
/* FILE pointer to retrieve data from .data file, then sort array */
void get_data()
{
    FILE *fPointer;
    if ((fPointer = fopen("tic-tac-toe.data", "r")) == NULL) // open file and check if it exists
    {
        g_print("Error: data file could not be opened\n");
        exit(1);
    }
    char raw_dataset[TOTAL][28]; // raw temporary data

    int i = 0;
    while (fgets(raw_dataset[i], 28, fPointer) != NULL) // each line in dataset contains 28 characters
    {
        i++;
    }
    fclose(fPointer); // close the file

    i = 0;
    int j, k = 0;
    do
    {
        k = 0;
        for (j = 0; j < 19; j += 2)
        {
            dataset[i][k] = raw_dataset[i][j]; // remove commas and '\n'
            k++;
        }
        i++;
    } while (i < TOTAL);

    i = 0;
    do
    {
        if (dataset[i][9] == 'n')
            total_negative++;
        else
            total_positive++;
        i++;
    } while (i < TOTAL);
    train_dataset();
}

/* train data for all three occurances (x, o, b) */
void train_dataset()
{
    int i = 0;

    for (i = 0; i < 9; i++)
        feature_possibility('x', i); // P(x | n)

    for (i = 0; i < 9; i++)
        feature_possibility('o', i); // P(o | n)

    for (i = 0; i < 9; i++)
        feature_possibility('b', i); // P(b | n)
}

/* calculate probability for training set */
void feature_possibility(char train_mark, int spot)
{
    int i = 0;

    double pos_occ = 0.0, neg_occ = 0.0, neg_probability = 0.0, pos_probability = 0.0;

    /*Get total number of positive and negative occurance when mark is at the spot*/
    for (i; i < TOTAL; i++)
    {
        if (dataset[i][spot] == train_mark)
        {
            if (dataset[i][9] == 'n')
                neg_occ++;
            else
                pos_occ++;
        }
    }

    /*conditional probability*/
    neg_probability = (neg_occ / total_negative);
    pos_probability = (pos_occ / total_positive);

    if (train_mark == 'x')
    {
        x_trained[spot] = neg_probability;
        x_trained[spot + 9] = pos_probability;
    }
    else if (train_mark == 'o')
    {
        o_trained[spot] = neg_probability;
        o_trained[spot + 9] = pos_probability;
    }
    else if (train_mark == 'b')
    {
        b_trained[spot] = neg_probability;
        b_trained[spot + 9] = pos_probability;
    }
}

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

/* Function to calculate probability */
double calc_probability(char input_mark, int move)
{
    double neg_probability, pos_probability;
    if (input_mark == 'x')
    {
        neg_probability = x_trained[move] * probability;     // neg probability
        pos_probability = x_trained[move + 9] * probability; // pos probability
        if (mark == 'x')                                     // if player == 1, computer is 'O', negative to win
            return neg_probability;
        else
            return pos_probability;
    }
    else if (input_mark == 'o')
    {
        neg_probability = o_trained[move] * probability;     // neg probability
        pos_probability = o_trained[move + 9] * probability; // pos probability
        if (mark == 'x')
            return neg_probability;
        else
            return pos_probability;
    }
}

/* Minimax function, return score */
int minimax(int maximise, char com_mark)
{
    int score, i, best_score, points;

    score = checkwin();

    if (score != 0) // game ended
    {
        if (score == 2)
        {
            return 0;
        }
        if (maximise == 1)
        {
            return -1;
        }
        else
        {
            return 1;
        }
    }

    if (maximise == 1) // maximizer
    {
        best_score = -1;
        for (i = 0; i < 9; i++)
        {
            if (square[i] != 'X' && square[i] != 'O')
            {
                square[i] = com_mark;
                points = minimax(0, com_mark);
                square[i] = i + '1';
                if (points > best_score)
                {
                    best_score = points;
                }
            }
        }
        return best_score;
    }

    else // minimizer
    {
        best_score = 1;
        for (i = 0; i < 9; i++)
        {
            if (square[i] != 'X' && square[i] != 'O')
            {
                square[i] = mark;
                points = minimax(1, com_mark);
                square[i] = i + '1';
                if (points < best_score)
                {
                    best_score = points;
                }
            }
        }
        return best_score;
    }
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
    gtk_grid_set_column_spacing(GTK_GRID(grid), 0);

    /*clear previous widgets*/
    GtkWidget *childs = gtk_widget_get_first_child(grid);
    while (childs != NULL)
    {
        gtk_grid_remove(GTK_GRID(grid), childs);
        childs = gtk_widget_get_first_child(grid);
    }

    space = gtk_label_new(""); // space on the LEFT of the screen to allow entries and button to be in the middle
    gtk_widget_set_size_request(space, 230, 640);
    gtk_grid_attach(GTK_GRID(grid), space, 0, 0, 11, 18);

    /* Entries for players to key in names for labels in the game screen */
    player1 = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(player1), "Player 1");
    gtk_grid_attach(GTK_GRID(grid), player1, 11, 4, 8, 1);

    player2 = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(player2), "Player 2");
    gtk_grid_attach(GTK_GRID(grid), player2, 11, 5, 8, 1);

    /* Start button */
    button = gtk_button_new_with_label("Start");
    g_signal_connect(button, "clicked", G_CALLBACK(two_player_gamescreen), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 11, 8, 10, 1);
}

/* initialise left and right labels (cannot attach yet as gamescreen function will remove all previous widgets) */
static void two_player_gamescreen(GtkWidget *widget, gpointer data)
{
    GtkWidget *player1_label, *player2_label, *childs;

    int x = 3, y = 3, i = 0;

    current_player = 1;
    /* Get player nammes from previous screen and set label names */
    player1_name = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(player1)));
    player2_name = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(player2)));
    left_label = gtk_label_new(player1_name);
    right_label = gtk_label_new(player2_name);

    gamescreen(); // standard gamescreen
}

/* Screen where user chooses difficulty before starting single player game */
/* Change window title, insert three buttons for three difficulty and include a back button*/
static void single_player(GtkWidget *widget, gpointer data)
{
    gtk_window_set_title(GTK_WINDOW(main_window), "Single Player"); // change window title
    gtk_grid_set_column_spacing(GTK_GRID(grid), 0);

    /*clear previous widgets*/
    GtkWidget *childs = gtk_widget_get_first_child(grid);
    while (childs != NULL)
    {
        gtk_grid_remove(GTK_GRID(grid), childs);
        childs = gtk_widget_get_first_child(grid);
    }

    space = gtk_label_new("");
    gtk_widget_set_size_request(space, 230, 560);
    gtk_grid_attach(GTK_GRID(grid), space, 0, 0, 11, 14);

    space = gtk_label_new("");
    gtk_widget_set_size_request(space, 230, 20);
    gtk_grid_attach(GTK_GRID(grid), space, 0, 16, 11, 1);

    label = gtk_label_new("Choose your difficulty");
    gtk_widget_set_size_request(label, 180, 40);
    gtk_grid_attach(GTK_GRID(grid), label, 11, 0, 10, 5);

    /* Difficulty buttons, each button links to different callback functions to change global variable 'difficulty' to change how computer plays */
    button = gtk_button_new_with_label("Easy");
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(easy), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 11, 6, 10, 1);

    button = gtk_button_new_with_label("Medium");
    g_signal_connect(button, "clicked", G_CALLBACK(medium), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 11, 7, 10, 1);

    button = gtk_button_new_with_label("Hard");
    g_signal_connect(button, "clicked", G_CALLBACK(hard), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 11, 8, 10, 1);

    /* Back button to go back to main menu */
    button = gtk_button_new_with_label("Back");
    g_signal_connect(button, "clicked", G_CALLBACK(main_menu), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 1, 15, 1, 1);
}

/* initialise left and right labels (cannot attach yet as gamescreen function will remove all previous widgets) */
void singleplayer_gamescreen()
{
    current_player = 3;
    left_label = gtk_label_new("You");       // x
    right_label = gtk_label_new("Computer"); // o

    gamescreen();
}

/* Called after every player move and if game is continuing */
void computer_move(int player_move)
{
    char com_mark = (mark == 'X') ? 'O' : 'X';
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
    square[best_move] = com_mark;                                                                                                                // update square array
    (mark == 'X') ? gtk_button_set_label(GTK_BUTTON(square_btn[best_move]), "O") : gtk_button_set_label(GTK_BUTTON(square_btn[best_move]), "X"); // change button label to computer mark
}

/* Common gamescreen to be used by both single player and two player mode */
/* Label for x and o, play again and main menu buttons, and 9 buttons to play tic tac toe */
void gamescreen()
{
    GtkWidget *childs;
    int x = 3, y = 3, i = 0;

    /*clear previous widgets*/
    childs = gtk_widget_get_first_child(grid);
    while (childs != NULL)
    {
        gtk_grid_remove(GTK_GRID(grid), childs);
        childs = gtk_widget_get_first_child(grid);
    }

    x_label = gtk_label_new("(X)");
    gtk_label_set_markup(GTK_LABEL(x_label), "<b>(X)</b>"); // player 1 start, bold x label
    o_label = gtk_label_new("(O)");

    /* space on the left */
    space = gtk_label_new("");
    gtk_widget_set_size_request(space, 120, 640);
    gtk_grid_attach(GTK_GRID(grid), space, 0, 0, 2, 16);

    /* play again button and main menu button */
    play_again_btn = gtk_button_new_with_label("Play again");
    g_signal_connect(play_again_btn, "clicked", G_CALLBACK(replay), NULL);
    gtk_grid_attach(GTK_GRID(grid), play_again_btn, 9, 2, 4, 1);

    main_menu_btn = gtk_button_new_with_label("Main Menu");
    g_signal_connect(main_menu_btn, "clicked", G_CALLBACK(main_menu), NULL);
    gtk_grid_attach(GTK_GRID(grid), main_menu_btn, 16, 2, 4, 1);

    /* Align labels */

    gtk_widget_set_size_request(left_label, 60, 40);
    gtk_widget_set_size_request(right_label, 60, 40);

    gtk_label_set_xalign(GTK_LABEL(x_label), 0.0);
    gtk_widget_set_valign(x_label, GTK_ALIGN_START);
    gtk_label_set_xalign(GTK_LABEL(o_label), 1.0);
    gtk_widget_set_valign(o_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), x_label, 3, 2, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), o_label, 24, 2, 3, 1);

    gtk_label_set_xalign(GTK_LABEL(left_label), 0.0);
    gtk_widget_set_valign(left_label, GTK_ALIGN_END);
    gtk_label_set_xalign(GTK_LABEL(right_label), 1.0);
    gtk_widget_set_valign(right_label, GTK_ALIGN_END);
    gtk_grid_attach(GTK_GRID(grid), left_label, 3, 1, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), right_label, 24, 1, 3, 1);

    /* nine buttons for tic tac toe game*/
    for (i = 0; i < 9; i++)
    {
        square_btn[i] = gtk_button_new_with_label(" ");
        gtk_widget_set_sensitive(square_btn[i], TRUE);
        gtk_widget_set_size_request(square_btn[i], 120, 120);
        g_signal_connect(square_btn[i], "clicked", G_CALLBACK(square_clicked), NULL);

        gtk_grid_attach(GTK_GRID(grid), square_btn[i], x, y, 7, 4);

        /*      Grid positions for buttons      */
        /*  |   3, 3   |   11,3   |   19,3   |  */
        /*  |   3, 7   |   11,7   |   19,7   |  */
        /*  |   3,11   |   11,11  |   19,11  |  */

        if (x == 19)
        {
            x = 3;
            y += 4;
        }
        else
        {
            x += 8;
        }
    }

    /* set play again to inactive, will be active after game has ended*/
    gtk_widget_set_sensitive(play_again_btn, FALSE);
}

/* When gamme ends, set all square buttons to inactive, set play again button to be active*/
void end_game()
{
    for (int i = 0; i < 9; i++)
    {
        gtk_widget_set_sensitive(square_btn[i], FALSE);
    }

    gtk_widget_set_sensitive(play_again_btn, TRUE);
}

/* Function be called back when play again button is presed */
/* Player 1 and 2 swap places, player 1 to be player 2 and vice versa */
/* reinitialise square array and buttons */
static void replay(GtkWidget *widget, gpointer data)
{
    /* Swap player 1 and 2 */
    const char *left_player = gtk_label_get_text((GTK_LABEL(left_label))), *right_player = gtk_label_get_text((GTK_LABEL(right_label)));

    /* Reinitialise square array */
    for (int i = 0; i < 9; i++)
    {
        gtk_widget_set_sensitive(square_btn[i], TRUE);
        gtk_button_set_label(GTK_BUTTON(square_btn[i]), " ");
        square[i] = i + '1';
    }

    /* Set play again button back to inactive */
    gtk_widget_set_sensitive(play_again_btn, FALSE);

    /* Update label */
    gtk_label_set_label(GTK_LABEL(left_label), right_player);
    gtk_label_set_label(GTK_LABEL(right_label), left_player);
    if (current_player == 2) // two player mode reset to player 1 start
    {
        current_player = 1;
        gtk_label_set_markup(GTK_LABEL(x_label), "<b>(X)</b>");
        gtk_label_set_markup(GTK_LABEL(o_label), "(O)");
    }
    else if (current_player == 3) // If single player mode, change player mark
    {
        mark = (mark == 'X') ? 'O' : 'X'; // if current player mark is X, change to O, player is player 2
        if (mark == 'O')                  // if player is player 2, computer starts first
        {
            gtk_label_set_markup(GTK_LABEL(x_label), "(X)");
            gtk_label_set_markup(GTK_LABEL(o_label), "<b>(O)</b>");
            computer_move(-1);
        }
        else
        {
            gtk_label_set_markup(GTK_LABEL(x_label), "<b>(X)</b>");
            gtk_label_set_markup(GTK_LABEL(o_label), "(O)");
        }
    }
}

/* main menu, allow player to choose between single player and two player mode */
static void main_menu(GtkWidget *widget, gpointer data)
{
    gtk_window_set_title(GTK_WINDOW(main_window), "Tic Tac Toe"); // change window title

    mark = 'X'; // reinitialise player mark

    /*clear previous widgets*/
    GtkWidget *childs = gtk_widget_get_first_child(grid);
    while (childs != NULL)
    {
        gtk_grid_remove(GTK_GRID(grid), childs);
        childs = gtk_widget_get_first_child(grid);
    }

    /* reinitialise square array */
    for (int i = 0; i < 9; i++)
    {
        square[i] = i + '1';
    }

    /* space on the left of screen */
    space = gtk_label_new("");
    gtk_widget_set_size_request(space, 230, 640);
    gtk_grid_attach(GTK_GRID(grid), space, 0, 0, 11, 16);

    /* space on top of screen */
    space = gtk_label_new("");
    gtk_widget_set_size_request(space, 180, 185);
    gtk_grid_attach(GTK_GRID(grid), space, 11, 0, 10, 5);

    button = gtk_button_new_with_label("Single Player");
    g_signal_connect(button, "clicked", G_CALLBACK(single_player), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 11, 5, 10, 1);

    button = gtk_button_new_with_label("Two Player");
    g_signal_connect(button, "clicked", G_CALLBACK(two_player), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 11, 6, 10, 1);

    button = gtk_button_new_with_label("Test Data");
    g_signal_connect(button, "clicked", G_CALLBACK(test_data), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 11, 7, 10, 1);

    /* Exit button */
    button = gtk_button_new_with_label("Quit");
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), main_window);
    gtk_grid_attach(GTK_GRID(grid), button, 11, 8, 10, 1);
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
    label = gtk_label_new(message); // message is showed as label

    // Ensure that the dialog box is destroyed when the user responds

    g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);

    // Add the label, and show everything

    gtk_box_append(GTK_BOX(content_area), label);

    gtk_widget_show(dialog);
}

/* Function to be called back when a square is clicked in game */
static void square_clicked(GtkWidget *widget, gpointer data)
{

    const gchar *btn_label = gtk_button_get_label(GTK_BUTTON(widget)); // get current label of button to check if it's empty
    const gchar *x_value = "X";
    const gchar *o_value = "O";

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

    if (*btn_label != *x_value && *btn_label != *o_value) // empty square
    {
        /* Two player */
        if (current_player == 1) // player 1
        {
            square[index] = 'x';
            gtk_button_set_label(GTK_BUTTON(widget), "X");
            k = checkwin(); // check if game has ended
            if (k == 1)     // game ended, win
            {
                showdialog("We have a winner!", "Player 1 win");
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
            square[index] = 'o';
            gtk_button_set_label(GTK_BUTTON(widget), "O");
            k = checkwin(); // check if game has ended
            if (k == 1)     // game ended, win
            {
                showdialog("We have a winner!", "Player 2 win");
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
            square[index] = mark;
            (mark == 'X') ? gtk_button_set_label(GTK_BUTTON(widget), "X") : gtk_button_set_label(GTK_BUTTON(widget), "O");
            k = checkwin(); // check if game has ended
            if (k == 1)     // game ended, win
            {
                showdialog("We have a winner!", "You win");
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

/* First main menu screen when user open app */
/* Initialise window and grid */
static void setup(GtkApplication *app, gpointer user_data)
{

    main_window = gtk_application_window_new(app);

    gtk_window_set_default_size(GTK_WINDOW(main_window), 640, 640);
    gtk_window_set_resizable(GTK_WINDOW(main_window), FALSE);

    /* construct the grid that is going pack buttons */
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 16);

    /* Pack the container in the window */
    gtk_window_set_child(GTK_WINDOW(main_window), grid);

    gtk_window_set_title(GTK_WINDOW(main_window), "Tic Tac Toe");

    space = gtk_label_new("");
    gtk_widget_set_size_request(space, 230, 640);
    gtk_grid_attach(GTK_GRID(grid), space, 0, 0, 11, 16);

    space = gtk_label_new("");
    gtk_widget_set_size_request(space, 180, 185);
    gtk_grid_attach(GTK_GRID(grid), space, 11, 0, 10, 5);

    button = gtk_button_new_with_label("Single Player");
    g_signal_connect(button, "clicked", G_CALLBACK(single_player), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 11, 5, 10, 1);

    button = gtk_button_new_with_label("Two Player");
    g_signal_connect(button, "clicked", G_CALLBACK(two_player), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 11, 6, 10, 1);

    button = gtk_button_new_with_label("Test Data");
    g_signal_connect(button, "clicked", G_CALLBACK(test_data), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 11, 7, 10, 1);

    button = gtk_button_new_with_label("Quit");
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), main_window);
    gtk_grid_attach(GTK_GRID(grid), button, 11, 8, 10, 1);

    gtk_widget_show(main_window);
}

/* return 0 to continue game, 1 for win, 2 for draw */
int checkwin()
{
    if (square[0] == square[1] && square[1] == square[2]) // horizontal
        return 1;

    else if (square[3] == square[4] && square[4] == square[5]) // horizontal
        return 1;

    else if (square[6] == square[7] && square[7] == square[8]) // horizontal
        return 1;

    else if (square[0] == square[3] && square[3] == square[6]) // vertical
        return 1;

    else if (square[1] == square[4] && square[4] == square[7]) // vertical
        return 1;

    else if (square[2] == square[5] && square[5] == square[8]) // vertical
        return 1;

    else if (square[0] == square[4] && square[4] == square[8]) // diagonal top left to bottom right
        return 1;

    else if (square[2] == square[4] && square[4] == square[6]) // diagonal top right to bottom left
        return 1;

    /*all squares taken up, game draw*/
    else if (square[0] != '1' && square[1] != '2' && square[2] != '3' &&
             square[3] != '4' && square[4] != '5' && square[5] != '6' && square[6] != '7' && square[7] != '8' && square[8] != '9')
        return 2;
    else
        return 0; // game continue
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
