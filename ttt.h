#define PLAYER_MARK 'X'
#define COM_MARK 'O'

extern char square[9]; // game state, reinitialised after every game has ended / user quit

extern int winning_row[3];

int checkwin();
void checkrow();
