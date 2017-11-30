
	/*
	** Example implementation of an HMM.
	** Uses a probability mass function for observables, meaning the
	** observed values are discrete.
	** The number of states and the number of observable (PMF) values
	** define the scale of the problem.
	** The initial, emission and transmission probabilities are
	** given and hardcoded.
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

	/* Food gesture problem.
	*/

#define	STATES		5	/* rest,utensil,bite,drink,other */
#define	PMF_VALUES	6	/* likelihood of having max motion on
				** each of acc xyz gyro ypr */
#define	INPUT_LENGTH	30	/* fixed time intervals */

main()

{
double	initial[STATES]={0.9,0.01,0.01,0.01,0.07};
double	emission[STATES][PMF_VALUES]={	\
		{0.2,0.2,0.2,0.2,0.2,0.2},	\
		{0.4,0.5,0.01,0.03,0.03,0.03},	\
		{0.04,0.03,0.03,0.05,0.05,0.8},	\
		{0.05,0.1,0.1,0.6,0.05,0.1},	\
		{0.1,0.1,0.7,0.03,0.03,0.04} };
double	transition[STATES][STATES]=	\
		{{0.072,0.338,0.364,0.093,0.133},	\
		{0.134,0.007,0.811,0.007,0.040},	\
		{0.309,0.364,0.141,0.022,0.165},	\
		{0.253,0.226,0.137,0.151,0.233},	\
		{0.285,0.256,0.292,0.167,0.0} };
int	input[INPUT_LENGTH]={	\
		0,1,0,0,2,4,0,0,2,2,	\
		5,5,4,4,3,2,3,4,0,5,	\
		3,2,0,5,5,5,0,3,1,3 };

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
  p[0][s]=initial[s]*emission[s][input[0]];
  plogs[0][s]=log2(initial[s])+log2(emission[s][input[0]]);
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
    p[i][s]=emission[s][input[i]]*n[max];
    plogs[i][s]=log2(emission[s][input[i]])+log2(n[max]);
    path[i][s]=max;
    }
  }

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
  printf("%d",output[i]);
printf("\n");

for (i=0; i<INPUT_LENGTH; i++)
  printf("%d",path[i]);
printf("\n");


}



