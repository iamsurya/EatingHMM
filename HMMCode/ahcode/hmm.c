
	/*
	** HMM for recognizing eating activities in the Shimmer data.
	** Two states, nonEA (0) and EA (1).
	** Four featres:  mad-roll, reg-roll, avg-sum-accel, manipulation.
	** Uses a PDF for observables, meaning the
	** observed values are continuous.
	** The initial, emission and transmission probabilities are
	** hardcoded and were calculated using hmm-stats.c.
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

#define	STATES		2	/* nonEA,EA */
#define	FEATURES	4	/* mad-roll,reg-roll,sum-accel,manip */
#define	MAX_INPUT	300	/* #segments of data during the day */




double determinant(int		n,		/* matrix size */
		   double	matrix[n][n])	/* nxn matrix */

{
double	**a,d,det;
int	*indx,i,j;

	/* allocate space for num-recipes matrix and vector */
a=(double **)calloc(n+1,sizeof(double *));
for (i=0; i<=n; i++)
  a[i]=(double *)calloc(n+1,sizeof(double));
indx=(int *)calloc(n+1,sizeof(int));

	/* copy matrix (indexed from zero) into a (indexed from one) */
for (i=0; i<n; i++)
  for (j=0; j<n; j++)
    a[i+1][j+1]=matrix[i][j];

	/* LU decomposition */
ludcmp(a,n,indx,&d);

	/* calculate determinant */
det=d*a[1][1];
for (i=2; i<=n; i++)
  det=det*a[i][i];

	/* free memory */
for (i=0; i<=n; i++)
  free(a[i]);
free(a);
free(indx);

return(det);
}



void matrix_inverse(int		n,		/* matrix size */
		    double	matrix[n][n],	/* input nxn matrix */
		    double	inverse[n][n])	/* output nxn matrix */

{
double	**dmatrix(),*dvector();
int	*ivector();
double	**a,**inv,d,*col;
int	*indx,i,j;

	/* allocate space for num-recipes matrices */
a=dmatrix(1,n,1,n);
inv=dmatrix(1,n,1,n);
indx=ivector(1,n);
col=dvector(1,n);

	/* copy matrix (indexed from zero) into a (indexed from one) */
for (i=0; i<n; i++)
  for (j=0; j<n; j++)
    a[i+1][j+1]=matrix[i][j];

	/* inverse matrix */
ludcmp(a,n,indx,&d);
for (j=1; j<=n; j++)
  {
  for (i=1; i<=n; i++)
    col[i]=0.0;
  col[j]=1.0;
  lubksb(a,n,indx,col);
  for (i=1; i<=n; i++)
    inv[i][j]=col[i];
  }

	/* copy inv (indexed from one) into inverse (indexed from zero) */
for (i=0; i<n; i++)
  for (j=0; j<n; j++)
    inverse[i][j]=inv[i+1][j+1];

	/* free memory */
free_dmatrix(a,1,n,1,n);
free_dmatrix(inv,1,n,1,n);
free_ivector(indx,1,n);
free_dvector(col,1,n);
}



double	multivar_norm_prob(int		n,
			   double	mean[n],
			   double	covar[n][n],
			   double	sample[n])
{
double	cov_inv[n][n];
double	mm[n];	/* used for matrix multiply */
double	det,numerator,e;
int	i,j;

matrix_inverse(n,covar,cov_inv);
det=determinant(n,covar);
numerator=0.0;
for (i=0; i<n; i++)
  {
  mm[i]=0.0;
  for (j=0; j<n; j++)
    mm[i]+=((sample[j]-mean[j])*cov_inv[j][i]);
  numerator+=(mm[i]*(sample[i]-mean[i]));
  }
e=exp((0.0-numerator)/2.0);
e=e*1.0/sqrt(pow(2.0*M_PI,(double)n)*det);
return(e);
}










int main(int argc, char *argv[])

{
double	initial[STATES]={0.889831,0.110169};
double	emission_mean[STATES][FEATURES]={	\
		{9.252833,0.480137,0.060118,635.858170},	\
		{9.263263,0.561186,0.050147,822.676658} };
double	emission_covar[STATES][FEATURES][FEATURES]={{	\
		{32.791796,1.034590,0.217099,167.248757},	\
		{1.034590,0.043850,0.006803,11.688444},	\
		{0.217099,0.006803,0.001736,-0.359132},	\
		{167.248757,11.688444,-0.359132,32721.655295}},	\
		{{15.576158,0.485388,0.086483,71.229134},	\
		{0.485388,0.025015,0.002682,12.521015},	\
		{0.086483,0.002682,0.000615,-0.593770},	\
		{71.229134,12.521015,-0.593770,40564.904782}} };
double	emission[STATES][FEATURES];
double	transition[STATES][STATES]={	\
		{0.966809,0.033191},	\
		{0.503157,0.496843} };
int	input[MAX_INPUT];
int	i,s,s_prev;
int	max;
double	p[MAX_INPUT][STATES];
double	n[STATES];
double	plogs[MAX_INPUT][STATES],nlogs[STATES];
int	output[MAX_INPUT];
int	path[MAX_INPUT][STATES];
char	filename[320];
int	start_index,end_index,total_data;
int	gt[MAX_INPUT];
double	feat[MAX_INPUT][4];
FILE	*fpt;


	/* parse hmm-data.txt for filename given on command line */
if (argc != 2)
  {
  printf("Usage:  hmm [filename]\n");
  printf("        filename can be shorthand version of shimmer PID\n");
  exit(0);
  }
if ((fpt=fopen("hmm-data.txt","r")) == NULL)
  {
  printf("Unable to open hmm-data.txt for reading\n");
  exit(0);
  }
total_data=0;
while (1)
  {
  i=fscanf(fpt,"%s %d %d %d %lf %lf %lf %lf",
	filename,&start_index,&end_index,&(gt[total_data]),
	&(feat[total_data][0]),&(feat[total_data][1]),
	&(feat[total_data][2]),&(feat[total_data][3]) );
  if (i != 8)
    break;
  if (strstr(filename,argv[1]) != NULL)
    {
    total_data++;
    if (total_data >= MAX_INPUT)
      {
      printf("MAX_INPUT (%d) exceeded. Too many segments that day?\n",
		MAX_INPUT);
      exit(0);
      }
    }
  }
fclose(fpt);
printf("%d total data\n",total_data);


	/* first forward probabilities depend on initial's */
for (s=0; s<STATES; s++)
  {
  p[0][s]=initial[s]*multivar_norm_prob(FEATURES,emission_mean[s],
	emission_covar[s],feat[0]);
  plogs[0][s]=log2(initial[s])+log2(multivar_norm_prob(FEATURES,
	emission_mean[s],emission_covar[s],feat[0]));
  }
	/* second->Nth forward probabilites calculated recursively */
for (i=1; i<total_data; i++)
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
    p[i][s]=multivar_norm_prob(FEATURES,emission_mean[s],
	emission_covar[s],feat[i])*n[max];
    plogs[i][s]=log2(multivar_norm_prob(FEATURES,emission_mean[s],
	emission_covar[s],feat[i]))+log2(n[max]);
    path[i][s]=max;
    }
  }

	/* display forward probabilities, both p and log(p) */
for (i=0; i<total_data; i++)
  {
  printf("%d  ",i);
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
  if (p[total_data-1][s] > p[total_data-1][max])
    max=s;
output[total_data-1]=max;
	/* it then recursively follows path backward that got here */
for (i=total_data-2; i>=0; i--)
  {
  output[i]=path[i+1][output[i+1]];
  }

	/* display state sequence */
for (i=0; i<total_data; i++)
  printf("%d",output[i]);
printf("\n");

}


