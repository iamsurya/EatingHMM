/* HMM to detect eating from all day wrist motion data.
** HMM parameters were calculated manually.
** Probability is calculated using the equation for multivariable normal distribution.
** Uses math from https://www.mathworks.com/help/stats/multivariate-normal-distribution.html
** and idiom.ucsd.edu/~rlevy/pmsl_textbook/chapters/pmsl_3.pdf
** needs -lgsl -lgslcblas -lm to build
** eg: gcc -o hmm.x hmm.c  -lgsl -lgslcblas -lm
*/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <gsl/gsl_linalg.h>

#define	FEATURES 	4
#define STATES		2	/* Eating and Non-Eating */
#define	MAX_LENGTH	300 /* Number of observations per participant */

/* Displays a square matrix of size n (matrix is a pointer) */
printmatrix(int n, double matrix[n][n])
{
	int j,k;

	printf("**************\n");
	for(j = 0; j < n; j++)
	{
		for(k = 0; k < n; k++)
			printf("%5.6f,", matrix[j][k]);
		printf("\n");
	}
}

/* Displays a gsl_matrix */
gsl_printmatrix(gsl_matrix * m)
{
	int j,k;

	printf("**************\n");
	for(j = 0; j < m->size1; j++)
	{
		for(k = 0; k < m->size2; k++)
			printf("%5.6f,", gsl_matrix_get(m,j,k));
		printf("\n");
	}
}

/* Calculates the inverse of the co-variance matrix and the determinant.
** Input
** n - size of square matrix
** covars - pointer to array of size [n][n]
** Output
** inv_covars - pointer to array of size [n][n]
** Return value
** det - determinant of matrix
** See https://lists.gnu.org/archive/html/help-gsl/2008-11/msg00000.html
** for an example on using GSL to find the inverse of a matrix
*/
double precalc(int n, double covars[n][n], double inv_covars[n][n])
{

	double det;
    int s, i, j;

	/* 2D to 1D cast */
	double * cvars = (double *) covars;
	double * inva = (double *) inv_covars;

	/* Allocate memory for GSL matrices */

    gsl_matrix_view m
         = gsl_matrix_view_array(cvars, n, n);
    gsl_matrix_view inv
         = gsl_matrix_view_array(inva,n,n);
    gsl_permutation * p = gsl_permutation_alloc (n);


    if(0)
    {
		printf("The matrix is\n");
		gsl_printmatrix(&m.matrix);
	}

	gsl_linalg_LU_decomp (&m.matrix, p, &s);
    gsl_linalg_LU_invert (&m.matrix, p, &inv.matrix);

    if(0)
    {
		printf("The inverse is\n");
		gsl_printmatrix(&inv.matrix);
    }

    gsl_permutation_free (p);

    det =  gsl_linalg_LU_det (&m.matrix, s);

	if(0) printf("Det: %6.3f\n", det);

    return det;
}

/* Returns a vector of n features from observation t */
/* Not needed if you store data as feat[length][num_feat] */
void getobs(int n, int l, int t, double INPUT[n][l], double X[n])
{
	int i;

	for (i = 0; i < n; i++)
		X[i] = INPUT[i][t];
}


/* Calculates probabilty given observation X, mean, covars(inverse) and det(covars) */
double multinormprob(int n, double X[n], double mean[n], double inv_covars[n][n], double det)
{

	double p;
	double mm[n]; /* Used for (x-u)' * inv_covars */
	int i, j;

	double invsum = 0.0;
	double e = 0.0;

	/* First calculate numerator for exp */
	for (i = 0; i < n; i++)
	{
		mm[i] = 0.0;

		for(j = 0; j < n; j++)
		{
			mm[i] += (X[j] - mean[j]) * inv_covars[j][i];
		}

		invsum += mm[i] * (X[i] - mean[i]);
	}

	/* Calculate exponential */
	e = exp((0 - invsum) / 2);
	p = e / (sqrt(pow(2*M_PI, (double) n)*det));

	return p;
}

void main(int argc, char * argv[])
{
	char filename[MAX_LENGTH] = "\0";
	char temp[MAX_LENGTH];
	int segmenttimes[2];
	int class[MAX_LENGTH];
	double feat[MAX_LENGTH][FEATURES];
	int total_data = 0;
	int i,s,prev_s, t,max, maxindex;
	int path[MAX_LENGTH][STATES];
	int output[MAX_LENGTH];

	/* HMM Parameters */
	const int LASTSTATE = STATES-1;
	double prior[STATES] = {0.889831,0.110169};

	double	transition[STATES][STATES]={	\
			{0.966809,0.033191},	\
			{0.503157,0.496843} };

	/* Gaussians (1 Gaussian per feature) */
	double means[STATES][FEATURES] = { \
			{9.252833,0.480137,0.060118,635.858170},	\
			{9.263263,0.561186,0.050147,822.676658} };

	double	emission_covars[STATES][FEATURES][FEATURES]={{	\
			{32.791796,1.034590,0.217099,167.248757},	\
			{1.034590,0.043850,0.006803,11.688444},	\
			{0.217099,0.006803,0.001736,-0.359132},	\
			{167.248757,11.688444,-0.359132,32721.655295}},	\
			{{15.576158,0.485388,0.086483,71.229134},	\
			{0.485388,0.025015,0.002682,12.521015},	\
			{0.086483,0.002682,0.000615,-0.593770},	\
			{71.229134,12.521015,-0.593770,40564.904782}} };

	/* Required for probability calculations. Precalculating speeds up the process */
	double inv_covars[STATES][FEATURES][FEATURES];

	double det_covars[STATES];

	double logprob[MAX_LENGTH][STATES];
	double slog[STATES]; /* Used for intermediate calculated probabilities */

	double logmnv, lognmax;

	/* Calculate the determinant and inverse covars matrices */
	for(s = 0; s < STATES; s++)
		det_covars[s] = precalc(FEATURES, emission_covars[s], inv_covars[s]);


	/* Load Observations */
	FILE * datafile = fopen("hmm-data.txt", "r");

	if(argc != 2)
	{
		printf("Usage ./hmm PID\nPID can be a number 2111 or P2123.\n");
		exit(0);
	}

	if(datafile == NULL)
	{
		printf("Could not find hmm-data.txt\n");
		exit(0);
	}

	/* Parse the hmm-data file to find data from the given PID */
	while(1)
	{
		i = fscanf(datafile, "%s %d %d %d %lf %lf %lf %lf", temp, &segmenttimes[0], &segmenttimes[1], &class[total_data],
										&feat[total_data][0], &feat[total_data][1], &feat[total_data][2],
										&feat[total_data][3]);

		if(i != 8) break;

		if(strstr(temp,argv[1]) != NULL)
			{
				if(filename[0] == 0) strcpy(filename, temp);
				total_data++;
				if(total_data >= MAX_LENGTH)
				{
					printf("Total segments > MAX_LENGTH (%d). Quitting. \n", MAX_LENGTH);
					exit(0);
				}
			}
	}

	// printf("%s loaded with %d segments.\n", filename, total_data);

	/* Run the HMM */
	/* first forward probabilities depend on initial's */
	t = 0;
	for (s=0; s<STATES; s++)
	  {
	  logprob[t][s]=log2(prior[s])+log2(multinormprob(FEATURES, feat[t], means[s], inv_covars[s], det_covars[s]));
	  }

		/* Forward Loop */
		/* We need to calculate probabilities of coming from all previous states, then find
		** their maximum value, and add it to the emission probability.
		** The most probable state (for path) is the one with the maximum value */

		for(t = 1; t < total_data; t++)
		{
			for(s = 0; s < STATES; s++)
			{
				/* First calculate probabilities of coming from each state */
				maxindex = 0;
				for(prev_s = 0; prev_s < STATES; prev_s++)
				{
					slog[prev_s] = logprob[t-1][prev_s] + log2(transition[prev_s][s]);
					if(slog[prev_s] > slog[maxindex]) maxindex = prev_s;
				}

				logmnv = log2(multinormprob(FEATURES, feat[t], means[s], inv_covars[s], det_covars[s]));
				lognmax = slog[maxindex];
				logprob[t][s] = logmnv + lognmax;

				/* Store which state brought me here */
				path[t][s] = maxindex;
			}

		}

/* Display log probabilities for all t */
if(0)
{
	for(t = 0; t < total_data; t++)
	{
		printf("%d\t", t);
		for(s = 0; s < STATES; s++)
			printf(s == LASTSTATE? "%lf\n" : "%lf\t", logprob[t][s]);
	}
}

/* Backward tracking */
/* First find the state with the highest probability */
max = 0;
for(s = 1; s < STATES; s++)
	if(logprob[total_data - 1][s] > logprob[total_data - 1][max])
		max = s;

output[total_data-1] = max;

for(t = total_data - 2; t >= 0; t--)
{
	output[t] = path[t+1][output[t+1]]; /* Op[t] is the state pointed out by the state with max probability in t+1.*/
}

	if(0)
	{
		printf("Gt Sequence:\n");
		for(i = 0; i < total_data; i++)	printf("%d", class[i]);
		printf("\n");
	}
		/* display state sequence */
	//printf("Hmm Sequence:\n");
	printf("%.5s: ", filename + strlen(filename) - strlen("P2015.shm"));
	for(i = 0; i < total_data; i++)	printf("%d", output[i]);
	printf("\n");

}

