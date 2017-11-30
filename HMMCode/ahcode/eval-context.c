
	/*
	** Evaluates context variables with respect to detection.
	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	MAX_RECORDS	1500

main(int argc, char *argv[])

{
FILE	*fpt;
char	text[320];
int	PID[MAX_RECORDS];
int	StartIndex[MAX_RECORDS],EndIndex[MAX_RECORDS];
char	StartTime[MAX_RECORDS][320],EndTime[MAX_RECORDS][320];
char	Weekday[MAX_RECORDS][320],MealType[MAX_RECORDS][320];
char	Location[MAX_RECORDS][320],Seconds[MAX_RECORDS][320];
char	Company[MAX_RECORDS][320];
char	Activity[MAX_RECORDS][320],Foods[MAX_RECORDS][320];
int	TotalEAs;
int	TotalDetects;
int	i,j,k;
int	start,match;
int	DPID[MAX_RECORDS],DStart[MAX_RECORDS],Detected[MAX_RECORDS];
int	TotalInContext,TotalFound;

fpt=fopen("database.txt","r");
TotalEAs=0;
while (1)
  {
  i=fscanf(fpt,"%s %d %d %s %s %s %s %s %s %s %s %s",
	text,&(StartIndex[TotalEAs]),&(EndIndex[TotalEAs]),
	StartTime[TotalEAs],EndTime[TotalEAs],
	Weekday[TotalEAs],
	MealType[TotalEAs],
	Location[TotalEAs],
	Seconds[TotalEAs],
	Company[TotalEAs],
	Activity[TotalEAs],
	Foods[TotalEAs]);
  if (i != 12)
    break;
  PID[TotalEAs]=atoi(&(text[1]));
  TotalEAs++;
  }
fclose(fpt);

fpt=fopen("results-eas.txt","r");
TotalDetects=0;
while (1)
  {
  i=fscanf(fpt,"%s %d %d",text,&start,&match);
  if (i != 3)
    break;
  text[strlen(text)-4]=0;
  DPID[TotalDetects]=atoi(&(text[strlen(text)-4]));
  DStart[TotalDetects]=start;
  if (match < 0)
    Detected[TotalDetects]=0;
  else
    Detected[TotalDetects]=1;
  TotalDetects++;
  }
fclose(fpt);

printf("%d total EAs, %d detects\n",TotalEAs,TotalDetects);

TotalInContext=0;	/* number of EAs having the selected context */
TotalFound=0;		/* number of them correctly detected by classifier */
for (i=0; i<TotalEAs; i++)
  {
  for (j=0; j<TotalDetects; j++)
    if (PID[i] == DPID[j]  &&  StartIndex[i] == DStart[j])
      break;
  if (j == TotalDetects)
    {
    if (0)
      printf("Skipping %d %d %s %s %s\n",PID[i],StartIndex[i],
	Weekday[i],MealType[i],Location[i]);
    continue;	/* this one not checked yet (couple files need work) */
    }

	/* next line tests for context variables of choice given as args */
  for (k=1; k<argc; k++)
    {
    if (strstr(Weekday[i],argv[k]) == NULL  &&
	strstr(MealType[i],argv[k]) == NULL  &&
	strstr(Location[i],argv[k]) == NULL)
      break;
    }
  if (k != argc)
    continue;

  TotalInContext++;
  if (Detected[j] == 1)
    TotalFound++;
  printf("%d %d %s %s %s => %d %d %d\n",PID[i],StartIndex[i],
	Weekday[i],MealType[i],Location[i],
	DPID[j],DStart[j],Detected[j]);
  }
printf("%d/%d => %.2lf\n",TotalFound,TotalInContext,
		(double)TotalFound/(double)TotalInContext);

}


