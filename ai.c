#include <stdio.h>
#include <stdlib.h>
#include "ai.h"

char dataset[TOTAL][10];

double probability;

/*Total number of positive and negative outcomes*/
double total_positive;
double total_negative;

double x_trained[18]; // 0-8 for negative, 9-17 for positive
double o_trained[18];
double b_trained[18];

/**** ML functions ****/
/* FILE pointer to retrieve data from .data file, then sort array */
void get_data()
{
    FILE *fPointer;
    total_negative = 0.0;
    total_positive = 0.0;
    probability = 1.0;
    if ((fPointer = fopen("tic-tac-toe.data", "r")) == NULL) // open file and check if it exists
    {
        printf("Error: data file could not be opened\n");
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
    for (i = 0; i < TOTAL; i++)
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
