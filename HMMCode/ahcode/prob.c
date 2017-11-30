#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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


