#include "minimax.h"
#include "ttt.h"

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