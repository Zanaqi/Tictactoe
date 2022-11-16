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

    int i = 0, k = 0, j = 0;
    while (fgets(raw_dataset[i], 28, fPointer) != NULL) // each line in dataset contains 28 characters
    {
        k = 0;
        for (j = 0; j < 19; j += 2)
        {
            dataset[i][k] = raw_dataset[i][j]; // remove commas and '\n'
            k++;
        }
        if (dataset[i][9] == 'n')
            total_negative++;
        else
            total_positive++;
        i++;
    }
    fclose(fPointer); // close the file

    printf("Training data using Naive Bayes Algorithm...\n");
    train_dataset();

    printf("Data initialisation complete\n");

    test_data();
}

void test_data()
{
    double positive_prior = total_positive / 958.0;
    double negative_prior = total_negative / 958.0;

    int positive_testing = total_positive * 0.2; // number of test positive
    int negative_testing = total_negative * 0.2; // number of test negative
    int confusion_matrix[4] = {0, 0, 0, 0};      // tp, fn, fp, tn
    int k = 0;                                   // values for confusion matrix

    int positive = total_positive * 0.8;
    float total_testing = positive_testing + negative_testing;
    float error = 0.0, accuracy = 0.0;
    double p_probability, n_probability, p_posterior, n_posterior;

    printf("\nTesting data with 20%% of data set\n\n");
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
                p_probability = p_probability * conditional_x[k + 9];
                n_probability = n_probability * conditional_x[k];
            }

            else if (dataset[i + positive][k] == 'o')
            {
                p_probability = p_probability * conditional_o[k + 9];
                n_probability = n_probability * conditional_o[k];
            }
            else if (dataset[i + positive][k] == 'b')
            {
                p_probability = p_probability * conditional_b[k + 9];
                n_probability = n_probability * conditional_b[k];
            }
        }

        if (p_probability > n_probability) // True positive
            confusion_matrix[0]++;

        else
        {
            error++;               // increase count of error output
            confusion_matrix[1]++; // False Negative
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
                p_probability = p_probability * conditional_x[k + 9];
                n_probability = n_probability * conditional_x[k];
            }

            else if (dataset[i][k] == 'o')
            {
                p_probability = p_probability * conditional_o[k + 9];
                n_probability = n_probability * conditional_o[k];
            }
            else if (dataset[i][k] == 'b')
            {
                p_probability = p_probability * conditional_b[k + 9];
                n_probability = n_probability * conditional_b[k];
            }
        }

        if (p_probability > n_probability)
        {
            error++;               // increase count of error
            confusion_matrix[2]++; // False Positive
        }

        else
            confusion_matrix[3]++; // True Negative
    }

    accuracy = ((total_testing - error) / total_testing) * 100;
    printf("Total tested                    %1.0f\n", total_testing);
    printf("True Positive (TP)              %d\n", confusion_matrix[0]);
    printf("False Negative (FN)             %d\n", confusion_matrix[1]);
    printf("False Positive (FP)             %d\n", confusion_matrix[2]);
    printf("True Negative (TN)              %d\n", confusion_matrix[3]);
    printf("Number of errors                %1.0f\n", error);
    printf("Accuracy                        %0.2f%%\n", accuracy);
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
