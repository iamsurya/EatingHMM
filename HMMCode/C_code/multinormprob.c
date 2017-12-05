/* Program to calculate multivariate normal probability distribution
** Uses math from https://www.mathworks.com/help/stats/multivariate-normal-distribution.html
** and idiom.ucsd.edu/~rlevy/pmsl_textbook/chapters/pmsl_3.pdf
** needs -lgsl -lgslcblas -lm to build
** eg: gcc -o mnp.x multinormprob.c  -lgsl -lgslcblas -lm
*/

#include<stdio.h>
#include<math.h>
#include <gsl/gsl_linalg.h>

#define FEATURES 4
#define STATES 2
#define INPUT_LENGTH 78
#define DEBUG 1

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

void getobs(int n, int l, int t, double INPUT[n][l], double X[n])
{
	int i;

	for (i = 0; i < n; i++)
		X[i] = INPUT[i][t];
}

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

void main()
{
	/* HMM Parameters */
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


/* Data from P2110 */
	double	input[FEATURES][INPUT_LENGTH]=	\
			{{3.034046,9.742308,5.961855,7.108932,11.694658,10.021533,8.540988,3.542312,14.798723,3.351082,4.475518,22.51903,13.624956,18.524024,7.386136,7.860689,12.849294,20.231657,13.606931,2.434677,10.031134,12.078637,3.949156,11.432502,11.769895,3.769912,2.992141,3.011466,1.245705,6.871301,5.76711,5.752354,3.591638,4.238911,8.165689,9.716103,9.243678,4.193059,3.602932,4.247639,4.473355,8.253877,8.373427,8.705992,4.569809,6.248093,3.836804,3.210246,28.749596,4.700124,1.19855,4.790599,7.784955,21.112784,22.6588,24.055417,12.189297,13.248328,12.110957,13.051306,14.664279,5.607597,7.727466,3.438529,16.886193,3.101057,9.994109,4.989154,4.130132,3.321645,5.415475,18.341725,15.914154,3.590517,5.579286,4.636124,1.805934,2.036047}, \
			{0.178866,0.422202,0.261348,0.375466,0.579741,0.624224,0.525627,0.192057,0.683077,0.20845,0.225645,0.521739,0.639335,0.684122,0.330345,0.479024,0.848609,0.83845,0.77978,0.186053,0.521778,0.632499,0.334794,0.480913,0.523705,0.207694,0.203351,0.211472,0.086352,0.35189,0.305961,0.435832,0.287263,0.320674,0.481394,0.353618,0.764939,0.245022,0.203838,0.344693,0.385237,0.533103,0.551308,0.247051,0.282946,0.312119,0.307387,0.165637,0.939293,0.211547,0.090007,0.172545,0.220611,0.838202,0.822811,0.863982,0.615269,0.613149,0.600638,0.668567,0.512773,0.162516,0.367366,0.150449,0.485333,0.171054,0.783968,0.313575,0.288605,0.235758,0.224649,0.629011,0.520572,0.202586,0.40366,0.233972,0.113385,0.159498}, \
			{0.034149,0.073126,0.04618,0.04941,0.06521,0.052656,0.046677,0.018548,0.162776,0.01832,0.036071,0.192337,0.197515,0.165903,0.062891,0.075511,0.157213,0.183228,0.105476,0.013104,0.061826,0.060889,0.019269,0.061719,0.082988,0.022358,0.021529,0.017136,0.008621,0.034145,0.04146,0.042829,0.03361,0.043704,0.05851,0.07192,0.083121,0.02309,0.021763,0.026347,0.026744,0.106746,0.048686,0.062738,0.029368,0.030301,0.025868,0.02166,0.294043,0.054307,0.033401,0.049873,0.117033,0.181628,0.152892,0.134454,0.060026,0.063017,0.040874,0.064798,0.069767,0.031657,0.048577,0.022157,0.121275,0.021944,0.044406,0.030434,0.01946,0.014456,0.033619,0.132101,0.0995,0.019163,0.035479,0.032463,0.007462,0.012698}, \
			{407.616264,536.558708,788.088253,934.607444,747.523093,1102.764827,628.780044,700.282562,489.132914,557.569051,468.461238,971.124429,485.964372,716.699664,386.300543,426.549272,387.718576,532.463543,579.326946,526.184038,652.279229,1032.286954,594.324732,586.95834,606.918434,904.904604,456.387915,550.336026,482.43268,991.007989,726.747238,513.434069,343.143154,327.277508,575.859743,692.301317,582.555662,676.561401,725.86368,683.510755,695.107467,537.912371,912.485786,402.654071,499.512604,620.05583,640.920114,429.999229,369.983609,362.244284,205.916743,261.155136,314.258846,547.307669,806.792078,732.849408,1042.144261,1136.174801,1052.835993,983.079516,709.664916,501.077995,618.733265,768.647874,861.138975,472.297083,1275.238472,1137.63617,675.123139,1063.819625,763.083256,676.760576,650.832912,868.567041,963.726351,785.19919,718.38103,573.427795} };

	double prob[STATES];

	/* Variables for program */
	int s, f;

	/* Precalculate the determinant and inverse covars matrices */
	for(s = 0; s < STATES; s++)
		det_covars[s] = precalc(FEATURES, emission_covars[s], inv_covars[s]);

	/* calculate probability */
	 double X[FEATURES];
	 getobs(FEATURES, INPUT_LENGTH, 77, input, X);

	for(s = 0; s < STATES; s++)
	{	
	 	prob[s] = multinormprob(FEATURES, X, means[s], inv_covars[s], det_covars[s]);
	 	printf("%6.6f\n", prob[s]);
	}

}