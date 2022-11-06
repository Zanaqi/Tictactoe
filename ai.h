#include "ttt.h"
#define TOTAL 958

extern char dataset[TOTAL][10];

extern double probability;

/*Total number of positive and negative outcomes*/
extern double total_positive;
extern double total_negative;

extern double x_trained[18]; // 0-8 for negative, 9-17 for positive
extern double o_trained[18];
extern double b_trained[18];

void get_data();
void train_dataset();
void feature_possibility(char mark, int spot);
double calc_probability(char input_mark, int move);
