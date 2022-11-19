#include "ttt.h"
#define TOTAL 958

extern double pos_posterior;
extern double neg_posterior;

/*Total number of positive and negative outcomes*/
extern double total_positive;
extern double total_negative;

extern double conditional_x[18]; // 0-8 for negative, 9-17 for positive
extern double conditional_o[18];
extern double conditional_b[18];

void get_data();
void train_dataset(char dataset[][10]);
void conditional_probability(char mark, int spot, char dataset[][10]);
void test_data(char dataset[][10]);
double calc_probability(char input_mark, int move, int store);
