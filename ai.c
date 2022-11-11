#include <stdio.h>
#include <stdlib.h>
#include "ai.h"

char dataset[TOTAL][10];

double pos_posterior;
double neg_posterior;

/*Total number of positive and negative outcomes*/
double total_positive;
double total_negative;

double conditional_x[18]; // 0-8 for negative, 9-17 for positive
double conditional_o[18];
double conditional_b[18];

/**** ML functions ****/
/* FILE pointer to retrieve data from .data file, then sort array */
void get_data()
{
    printf("Tic Tac Toe\n\n");
    printf("Getting data from dataset...\n");
    FILE *fPointer;
    total_negative = 0.0;
    total_positive = 0.0;
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

    printf("Training data using Naive Bayes Algorithm...\n");
    train_dataset();

    printf("Data initialisation complete\n");
}

/* train data for all three occurances (x, o, b) */
void train_dataset()
{
    int i = 0;

    for (i = 0; i < 9; i++)
        conditional_probability('x', i); // P(x | n)

    for (i = 0; i < 9; i++)
        conditional_probability('o', i); // P(o | n)

    for (i = 0; i < 9; i++)
        conditional_probability('b', i); // P(b | n)
}

/* calculate probability for training set */
void conditional_probability(char train_mark, int spot)
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
        conditional_x[spot] = neg_probability;
        conditional_x[spot + 9] = pos_probability;
    }
    else if (train_mark == 'o')
    {
        conditional_o[spot] = neg_probability;
        conditional_o[spot + 9] = pos_probability;
    }
    else if (train_mark == 'b')
    {
        conditional_b[spot] = neg_probability;
        conditional_b[spot + 9] = pos_probability;
    }
}

/* Function to calculate posterior probability */
double calc_probability(char input_mark, int move, int store) // store = 1 to store in global var
{
    /* Store current posterior probabilities in temporary variables */
    double temp_neg, temp_pos;

    temp_neg = neg_posterior;
    temp_pos = pos_posterior;

    if (input_mark == 'x')
    {
        temp_neg *= conditional_x[move];     // neg probability
        temp_pos *= conditional_x[move + 9]; // pos probability
    }
    else if (input_mark == 'o')
    {
        temp_neg *= conditional_o[move];     // neg probability
        temp_pos *= conditional_o[move + 9]; // pos probability
    }

    temp_neg = temp_neg / (temp_neg + temp_pos);
    temp_pos = temp_pos / (temp_neg + temp_pos);

    if (store == 1)
    {
        neg_posterior = temp_neg;
        pos_posterior = temp_pos;
    }
    return temp_neg; // player mark is 'x', computer is 'O', negative probability for computer to win
}
