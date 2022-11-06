#include "ttt.h"

char mark;           // player mark
int difficulty;      // 1 - easy, 2 - medium, 3 - hard
int current_player;  // 1 and 2 for two player, 3 for single player
char square[9];      // game state, reinitialised after every game has ended / user quit
int starting_player; // starting player for the game, either 1 or 2, change after every round
int player1_score;   // score tracker for player 1
int player2_score;   // score tracker for player 2

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