#include <gtk/gtk.h>

#define BUTTON_WIDTH 160
#define BUTTON_HEIGHT 50

extern GtkWidget *main_window;
extern GtkWidget *grid;
extern GtkWidget *layout;
extern GtkWidget *backgnd;
extern GtkWidget *button;
extern GtkWidget *space;
extern GtkWidget *label;
extern GtkWidget *player1;
extern GtkWidget *player2;
extern GtkWidget *replay_btn;
extern GtkWidget *main_menu_btn;
extern GtkWidget *left_label;
extern GtkWidget *right_label;
extern GtkWidget *x_label;
extern GtkWidget *o_label;
extern GtkWidget *square_btn[9];
extern GtkWidget *left_score_label;
extern GtkWidget *right_score_label;
extern GtkWidget *tie_label;
extern GtkWidget *tie_score_label;

extern const char *player1_name;
extern const char *player2_name;

void clear_layout();
void showdialog(const char *title, const char *message);
void placeLabel(GtkWidget *widget, int x, int y, int width, int height, float xalign, GtkAlign valign);
void placeWidget(GtkWidget *widget, int x, int y, int width, int height);