	/* 
	** This file checks if the math to calculate probability
	** from gaussian distribution is correct
	** compile: gcc -o probcheck.x probcheck -lm
	** usage ./probcheck val feat
	** eg: ./probcheck.x 100 0 should give you a 12.5% probability
	*/

#include<stdio.h>
#include<math.h>
#include <stdlib.h>

#define SQR(x) ((x)*(x))

double mean[4] = {100,200,300,400};
double var[4] = {10,20,30,40};

double gaussprob(double obsval, int feat)
{
	double gprob;
	double VAR = var[feat];
	double MEAN = mean[feat];

	double exponent = (0-SQR(obsval-MEAN))/(2*VAR);
	gprob = (1/(sqrt(2*M_PI*VAR))) * pow(M_E,exponent);
	return gprob;
}

void main(int argc, char * argv[])
{
double val = atof(argv[1]);
int fval = atoi(argv[2]);

double cval = (1.0 / sqrt(2.0*M_PI*var[fval]))*pow(M_E, 0.0 - SQR(val - mean[fval]) / (2.0*var[fval]));

printf("%f\t%f\n", gaussprob(val, fval), cval);

}



