
	/*
	** Calculates HMM probabilities (initial, transition and
	** observation) from automatically segmented shimmer data
	** (segmented by peak detector).  The segments, their GT
	** identities and feature values are stored in hmm-data.txt.
	** This program parses that file to calculate all the
	** probabilities.
	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DATA 40000

int main(int argc, char *argv[])

{
char	**filename;
int	*startindex,*endindex;
int	*class;
double	**feat;
FILE	*fpt;
int	i,j,k,total_data;
int	total_training_seq;
int	prior[2],trans[2][2];
double	obs_mean[2][4],covar[2][4][4];
int	obs_freq[2];

	/* allocate memory */
filename=(char **)calloc(MAX_DATA,sizeof(char *));
for (i=0; i<MAX_DATA; i++)
  filename[i]=(char *)calloc(320,sizeof(char));
startindex=(int *)calloc(MAX_DATA,sizeof(int));
endindex=(int *)calloc(MAX_DATA,sizeof(int));
class=(int *)calloc(MAX_DATA,sizeof(int));
feat=(double **)calloc(MAX_DATA,sizeof(double *));
for (i=0; i<MAX_DATA; i++)
  feat[i]=(double *)calloc(4,sizeof(double));

	/* read data */
fpt=fopen("hmm-data.txt","rb");
total_data=0;
while (1)
  {
  i=fscanf(fpt,"%s %d %d %d %lf %lf %lf %lf",
	filename[total_data],
	&(startindex[total_data]),&(endindex[total_data]),
	&(class[total_data]),
	&(feat[total_data][0]),&(feat[total_data][1]),
	&(feat[total_data][2]),&(feat[total_data][3]));
  if (i != 8)
    break;
  total_data++;
  }
fclose(fpt);
printf("%d total data\n",total_data);

	/* calculate prior probabilities */
prior[0]=prior[1]=0;
total_training_seq=0;
for (i=0; i<total_data; i++)
  {
  if (i == 0  ||  strcmp(filename[i],filename[i-1]) != 0)
    {
    total_training_seq++;
    prior[class[i]]++;
    if (0) // class[i] == 1) /* tested */
      printf("%s\n",filename[i]);
    }
  }
printf("%d training sequences\n",total_training_seq);
printf("priors %d %d  %lf %lf\n",prior[0],prior[1],
	(double)prior[0]/(double)total_training_seq,
	(double)prior[1]/(double)total_training_seq);

	/* calculate transition probabilities */
trans[0][0]=trans[0][1]=trans[1][1]=trans[1][0]=0;
for (i=0; i<total_data; i++)
  {
  if (i == total_data-1  ||  strcmp(filename[i],filename[i+1]) != 0)
    continue;	/* at end of sequence, not a transition */
  trans[class[i]][class[i+1]]++;
  }
printf("trans %d %d  %d %d   %lf %lf  %lf %lf\n",
	trans[0][0],trans[0][1],trans[1][0],trans[1][1],
	(double)(trans[0][0])/(double)(trans[0][0]+trans[0][1]),
	(double)(trans[0][1])/(double)(trans[0][0]+trans[0][1]),
	(double)(trans[1][0])/(double)(trans[1][0]+trans[1][1]),
	(double)(trans[1][1])/(double)(trans[1][0]+trans[1][1]) );

	/* calculate observation means */
for (i=0; i<2; i++)	/* state (0=non, 1=EA) */
  {
  obs_freq[i]=0;
  for (j=0; j<4; j++)	/* feature */
    {
    obs_mean[i][j]=0.0;
    for (k=0; k<4; k++)
      covar[i][j][k]=0.0;
    }
  }
for (i=0; i<total_data; i++)
  {
  for (j=0; j<4; j++)
    {
    obs_mean[class[i]][j]+=feat[i][j];
    }
  obs_freq[class[i]]++;
  }
for (i=0; i<2; i++)
  for (j=0; j<4; j++)
    obs_mean[i][j]/=(double)obs_freq[i];
printf("state freq %d %d\n",obs_freq[0],obs_freq[1]);
printf("obs means:\n");
for (i=0; i<2; i++)
  {
  for (j=0; j<4; j++)
    printf(" %lf",obs_mean[i][j]);
  printf("\n");
  }

	/* calculate observation covariances */
for (i=0; i<total_data; i++)
  {
  for (j=0; j<4; j++)
    for (k=0; k<4; k++)
      covar[class[i]][j][k]+=( (feat[i][j]-obs_mean[class[i]][j])*
		(feat[i][k]-obs_mean[class[i]][k]) );
  }
for (i=0; i<2; i++)
  for (j=0; j<4; j++)
    for (k=0; k<4; k++)
      covar[i][j][k]/=(double)(obs_freq[i]-1);
printf("obs covars:\n");
for (i=0; i<2; i++)
  {
  for (j=0; j<4; j++)
    {
    for (k=0; k<4; k++)
      printf(" %lf",covar[i][j][k]);
    printf("\n");
    }
  printf("\n");
  }

}

