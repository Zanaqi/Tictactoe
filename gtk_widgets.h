#include <gtk/gtk.h>

#define BUTTON_WIDTH 160
#define BUTTON_HEIGHT 50

extern GtkWidget *main_window;
extern GtkWidget *layout;

struct Gamedata
{
    GtkWidget *player1_entry;
    GtkWidget *player2_entry;
    GtkWidget *replay_btn;
    GtkWidget *square_btn[9];
    GtkWidget *left_label;
    GtkWidget *right_label;
    GtkWidget *x_label;
    GtkWidget *o_label;
    GtkWidget *left_score_label;
    GtkWidget *right_score_label;
    GtkWidget *tie_label;
    GtkWidget *tie_score_label;

    const char *player1_name;
    const char *player2_name;
    int player1_score; // score tracker for player 1
    int player2_score; // score tracker for player 2
    int tie_score;     // score tracker for tie

    int current_player;  // 1 and 2 for two player, 3 for single player
    int starting_player; // starting player for the game, either 1 or 2, change after every round
    int difficulty;
    int gamestate; // checkwin value
};

void clear_layout(char *backgnd_resource);
void showdialog(const char *title, const char *message);
void placeWidget(GtkWidget *widget, int x, int y, int width, int height);
void labels_state(int state, struct Gamedata *gamedata);