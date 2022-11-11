#include "minimax.h"
#include "ttt.h"

#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)

/* Minimax function, return score */
int minimax(int maximise, int depth)
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
            return (-10 + depth);
        }
        else
        {
            return (10 - depth);
        }
    }

    if (maximise == 1) // maximizer
    {
        best_score = -100;
        for (i = 0; i < 9; i++)
        {
            if (square[i] != 'X' && square[i] != 'O')
            {
                square[i] = COM_MARK;
                points = MAX(best_score, minimax(0, depth + 1));
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
        best_score = 100;
        for (i = 0; i < 9; i++)
        {
            if (square[i] != 'X' && square[i] != 'O')
            {
                square[i] = PLAYER_MARK;
                points = MIN(best_score, minimax(1, depth + 1));
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