#include<stdio.h>
#include<math.h>

void main(){
    FILE *fp=NULL;
    FILE *gnuplot=NULL;

    // fp = fopen("circle.txt", "w");

    // double r;
    // double x,y,x0,y0;

    // printf("Enter the radius of the circle to be plotted: ");
    // scanf("%lf",&r);
    // printf("Enter the x and y-coordinates of the center: ");
    // scanf("%lf%lf",&x0,&y0);

    // for(y=y0-r;y<=y0+r;y=y+0.1)
    // {
    //     x=sqrt(r*r-(y-y0)*(y-y0))+x0; 
    //     fprintf(fp,"%lf\t %lf\n",x,y);
    // }

    // for(y=y0+r;y>=y0-r;y=y-0.1)
    // {
    //     x=-sqrt(r*r-(y-y0)*(y-y0))+x0; 
    //     fprintf(fp,"%lf\t %lf\n",x,y);
    // }

    gnuplot=popen("gnuplot -persist", "w");

    if(gnuplot != NULL)
        fprintf(gnuplot, "plot \"test.txt\" w l\n");

}