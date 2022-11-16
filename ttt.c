#include "ttt.h"

char square[9]; // squares state, reinitialised after every game has ended / user quit
int winning_row[3];

void checkrow()
{
    // horizontal
    if (square[0] == square[1] && square[1] == square[2])
    {
        winning_row[0] = 0;
        winning_row[1] = 1;
        winning_row[2] = 2;
    }

    // horizontal
    else if (square[3] == square[4] && square[4] == square[5])
    {
        winning_row[0] = 3;
        winning_row[1] = 4;
        winning_row[2] = 5;
    }

    // horizontal
    else if (square[6] == square[7] && square[7] == square[8])
    {
        winning_row[0] = 6;
        winning_row[1] = 7;
        winning_row[2] = 8;
    }

    // vertical
    else if (square[0] == square[3] && square[3] == square[6])
    {
        winning_row[0] = 0;
        winning_row[1] = 3;
        winning_row[2] = 6;
    }

    // vertical
    else if (square[1] == square[4] && square[4] == square[7])
    {
        winning_row[0] = 1;
        winning_row[1] = 4;
        winning_row[2] = 7;
    }

    // vertical
    else if (square[2] == square[5] && square[5] == square[8])
    {
        winning_row[0] = 2;
        winning_row[1] = 5;
        winning_row[2] = 8;
    }

    // diagonal top left to bottom right
    else if (square[0] == square[4] && square[4] == square[8])
    {
        winning_row[0] = 0;
        winning_row[1] = 4;
        winning_row[2] = 8;
    }

    // diagonal top right to bottom left
    else if (square[2] == square[4] && square[4] == square[6])
    {
        winning_row[0] = 2;
        winning_row[1] = 4;
        winning_row[2] = 6;
    }
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