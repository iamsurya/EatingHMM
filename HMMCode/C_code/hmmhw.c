/* This program contains code to implement the forward and backward algorithms
used by HMMs. We create a two state HMM shown in http://cecas.clemson.edu/~ahoover/ece854/refs/Gonze-ViterbiAlgorithm.pdf
where the states are (H or L) (0 or 1) and there are 4 PMF values for one observation (A,C,G,T) (0,1,2,3).
The prior, transition and emission probabilities are given, but we convert them to log2 values
because the probabilities become very small very quickly.
This program is also given as homework (lab 7) at http://cecas.clemson.edu/~ahoover/ece854/labs/lab7-hmm.pdf
*/

#include <stdio.h>
#include <math.h>

#define MAX(x,y) (((x) > (y)) ? (x) : (y))
#define MIN(x,y) (((x) < (y)) ? (x) : (y))

#define STATES 2
#define PMF_VALS 4
#define INPUT_LENGTH 9

int main()
{
int s, t, prev_s;
double maxval;
int	   maxindex; 
int max;
int output[INPUT_LENGTH];

double plogs[STATES][INPUT_LENGTH]; // We only calculate log2 probabilities
int		path[INPUT_LENGTH][STATES];
double slog[STATES];

double prior[STATES] = {0.5, 0.5};
double transition[STATES][STATES] = {
		{0.5, 0.5},
		{0.4, 0.6}};
double emission[STATES][PMF_VALS] = {
		{0.2,0.3,0.3,0.2},
		{0.3,0.2,0.2,0.3}};

// int input[INPUT_LENGTH] = {2,2,1,0,1,3,2,0,0}; // GGCACTGAA
int input[INPUT_LENGTH] = {3,1,0,2,1,2,2,1,3}; // CAGCGGCT

char NUM2CHAR_[STATES] = {'H', 'L'};

/* First calculate probabilities for state 0 */
t = 0;
printf("%d ", t);
for(s = 0; s < STATES; s++)
{
	plogs[s][t] = log2(prior[s]) + log2(emission[s][input[t]]);
	printf("%e\t", plogs[s][t]);
}
printf("\n");

/* Forward Loop */
/* We need to calculate probabilities of coming from all previous states, then find
** their maximum value, and add it to the emission probability.
** The most probable state (for path) is the one with the maximum value */
for(t = 1; t < INPUT_LENGTH; t++)
{
	printf("%d ", t);
	for(s = 0; s < STATES; s++)
	{
		maxval = -99999999999999999999999999999999.0;
		maxindex = 0;

		/* First calculate probabilities of coming from each state */
		for(prev_s = 0; prev_s < STATES; prev_s++)
		{
			slog[prev_s] = plogs[prev_s][t-1] + log2(transition[prev_s][s]);
			if(slog[prev_s] > maxval)
			{
				maxval = slog[prev_s];
				maxindex = prev_s;
			}
		}

		plogs[s][t] = log2(emission[s][input[t]]) + maxval;
		printf("%e(%e, %e)\t", plogs[s][t], log2(emission[s][input[t]]), maxval);
	}
	path[t][s] = maxindex; //(plogs[0][t] > plogs[1][t]? 0 : 1);
	//printf("%d\n", path[t][s]);
}

	/* backward algorithm starts at max probability at end */
max=0;
for (s=1; s<STATES; s++)
  if (pow(2,plogs[INPUT_LENGTH-1][s]) > pow(2,plogs[INPUT_LENGTH-1][max]))
    max=s;
output[INPUT_LENGTH-1]=max;
	/* it then recursively follows path backward that got here */
for (t=INPUT_LENGTH-2; t>=0; t--)
  {
  output[t]=path[t+1][output[t+1]];
  }

for(t = 0; t<INPUT_LENGTH; t++)
{
	printf("%c", NUM2CHAR_[output[t]]);
}

printf("\n");
}