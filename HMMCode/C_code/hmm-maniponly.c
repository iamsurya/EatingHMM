
	/*
	** Example implementation of an HMM.
	** Converted from hmm-foods.c by Adam Hoover
	** Uses a probability density function for observables, meaning the
	** observed values are continuous.
	** The number of states and the number of observable (PMF) values
	** define the scale of the problem.
	** The initial, transmission probabilities are
	** given and hardcoded.
	** The emission probabilities are represented as means and variances.
	** We do not use covariances in this example.
	** The input is a sequence of observed values.
	** Viterbi decoding (forward algorithm then backward algorithm)
	** is used to determine the most likely sequence of states that
	** produced the input sequence of observed values.
	** Calculates probabilities using raw p and log(p), the latter
	** of which is necessary for numerical stability of long sequences.
	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define SQR(x)		((x)*(x))

	/* Food gesture problem.
	*/

#define	STATES		2	/* Eating / Non Eating */
#define	NUM_FEATURES	1	/* Number of features in our data */
#define	INPUT_LENGTH	78	/* length of observation sequence */

double	vars[NUM_FEATURES][STATES] = \
		 {{32722, 40565}};

double	means[NUM_FEATURES][STATES] = \
		 {{635.858170, 822.676658}};

double	input[NUM_FEATURES][INPUT_LENGTH]=	\
		{{407.616264,536.558708,788.088253,934.607444,747.523093,1102.764827,628.780044,700.282562,489.132914,557.569051,468.461238,971.124429,485.964372,716.699664,386.300543,426.549272,387.718576,532.463543,579.326946,526.184038,652.279229,1032.286954,594.324732,586.95834,606.918434,904.904604,456.387915,550.336026,482.43268,991.007989,726.747238,513.434069,343.143154,327.277508,575.859743,692.301317,582.555662,676.561401,725.86368,683.510755,695.107467,537.912371,912.485786,402.654071,499.512604,620.05583,640.920114,429.999229,369.983609,362.244284,205.916743,261.155136,314.258846,547.307669,806.792078,732.849408,1042.144261,1136.174801,1052.835993,983.079516,709.664916,501.077995,618.733265,768.647874,861.138975,472.297083,1275.238472,1137.63617,675.123139,1063.819625,763.083256,676.760576,650.832912,868.567041,963.726351,785.19919,718.38103,573.427795} };



/* Calculates probability given a value, feature and state */
double gaussprob(double obsval, int feat, int state)
{
        double VAR = vars[feat][state];
        double MEAN = means[feat][state];

        double exponent = (0-SQR(obsval-MEAN))/(2*VAR);
        double gprob = (1/(sqrt(2*M_PI*VAR))) * pow(M_E,exponent);
	
	//printf("val:%f m:%f var %f gp: %f ", obsval, MEAN, VAR, gprob);       

	return gprob;
}

/* Calculates cumulative probability for time t in state s */
double cumuprob(int t, int s)
{
	int i = 0;
	double result = 1.0;
	
	//printf("t: %d s: %d ", t,s);
	result = gaussprob(input[0][t], i, s);
	//printf("v: %f p(o_t|s): %f\n", input[1][t], result);
	return result;
}

main()
{
double	initial[STATES]={0.8898, 0.1102};

double	transition[STATES][STATES]=	\
		{{0.966809,0.033191},	\
		{0.503157, 0.496843} };

int	i,s,s_prev;
int	max;
double	p[INPUT_LENGTH][STATES];
double	n[STATES];
double	plogs[INPUT_LENGTH][STATES],nlogs[STATES];
int	output[INPUT_LENGTH];
int	path[INPUT_LENGTH][STATES];

	/* first forward probabilities depend on initial's */
for (s=0; s<STATES; s++)
  {
  p[0][s]=initial[s]*cumuprob(0,s);
  plogs[0][s]=log2(initial[s])+log2(cumuprob(0,s));
  }

	/* second->Nth forward probabilites calculated recursively */
for (i=1; i<INPUT_LENGTH; i++)
  {
  for (s=0; s<STATES; s++)
    {
    for (s_prev=0; s_prev<STATES; s_prev++)
      {
      n[s_prev]=transition[s_prev][s]*p[i-1][s_prev];
      nlogs[s_prev]=log2(transition[s_prev][s])+log2(p[i-1][s_prev]);
      }
    max=0;
    for (s_prev=1; s_prev<STATES; s_prev++)
      if (n[s_prev] > n[max])
        max=s_prev;
    p[i][s]=cumuprob(i,s)*n[max];
    plogs[i][s]=log2(cumuprob(i,s))+log2(n[max]);
    path[i][s]=max;
    }
  }

printf("Forward Complete\n");
/* display forward probabilities, both p and log(p) */
if(0)
for (i=0; i<INPUT_LENGTH; i++)
  {
  for (s=0; s<STATES; s++)
    {
    printf("%lf (%e)",plogs[i][s],p[i][s]);
    if (s == STATES-1)
      printf("\n");
    else
      printf("\t");
    }
  }

	/* backward algorithm starts at max probability at end */
max=0;
for (s=1; s<STATES; s++)
  if (p[INPUT_LENGTH-1][s] > p[INPUT_LENGTH-1][max])
    max=s;
output[INPUT_LENGTH-1]=max;
	/* it then recursively follows path backward that got here */
for (i=INPUT_LENGTH-2; i>=0; i--)
  {
  output[i]=path[i+1][output[i+1]];
  }

	/* display state sequence */
for (i=0; i<INPUT_LENGTH; i++)
  printf("%d,",output[i]+1); // Matches Matlab indexing
printf("\n");

}

