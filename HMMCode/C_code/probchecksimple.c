	/* 
	** This file checks if the math to calculate probability
	** from gaussian distribution is correct
	** compile: gcc -o probcheck.x probcheck -lm
	** usage ./probcheck val mean var
	** eg: ./probcheck.x 100 100 10  should give you a 12.5% probability
	*/

#include<stdio.h>
#include<math.h>
#include <stdlib.h>

#define SQR(x) ((x)*(x))

double gaussprob(double obsval, double MEAN, double VAR)
{
	double gprob;
	double exponent = (0-SQR(obsval-MEAN))/(2*VAR);
	gprob = (1/(sqrt(2*M_PI*VAR))) * pow(M_E,exponent);
	return gprob;
}

void main(int argc, char * argv[])
{
double val = atof(argv[1]);
double mean = atoi(argv[2]);
double var = atoi(argv[3]);

double cval = (1.0 / sqrt(2.0*M_PI*var))*pow(M_E, 0.0 - SQR(val - mean) / (2.0*var));

printf("%f\n", cval);

}


