
	/*
	** This program parses a -events.txt file and creates database
	** entries for all the eating activities.  To run it on all the
	** files, a script containing a command for each file can be run.
	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EA	20

int main(int argc, char *argv[])

{
FILE	*fpt;
int	i,TotalEvents,TimeOffset;
char	PID[320],text[320],texttime[320],texttime2[320],textdate[320];
int	EventStart[MAX_EA];
int	EventEnd[MAX_EA];
char	EventTimeStart[MAX_EA][80],EventTimeEnd[MAX_EA][80];
char	EventName[MAX_EA][80];
char	EventLocation[MAX_EA][80];
char	EventHelpings[MAX_EA][80];
char	EventCompany[MAX_EA][80];
char	EventFoods[MAX_EA][320];
char	EventActivities[MAX_EA][320];
int	y,m,d,weekday;
char	Weekday[7][32]={"Sunday","Monday","Tuesday","Wednesday", \
			"Thursday","Friday","Saturday"};


if (argc != 2)
  {
  printf("Usage:  meals [P####-events.txt]\n");
  exit(0);
  }
if ((fpt=fopen(argv[1],"r")) == NULL)
  {
  printf("Unable to open %s for reading\n",argv[1]);
  exit(0);
  }

	/* PID is assumed to be first 5 characters of filename */
strncpy(PID,argv[1],5);
PID[5]='\0';

TotalEvents=0;
while (1)
  {
  i=fscanf(fpt,"%s",text);
  if (i != 1)
    break;
  if (strcmp(text,"START") == 0)
    {
    i=fscanf(fpt,"%s %s",textdate,texttime);
    textdate[4]=textdate[7]=0;	/* null out hyphens */
    y=atoi(&(textdate[0]));
    m=atoi(&(textdate[5]));
    d=atoi(&(textdate[8]));
    weekday=(d+=m<3?y--:y-2,23*m/9+d+4+y/4-y/100+y/400)%7; 
    texttime[2]=texttime[5]=0;	/* null out colons */
		/* units of TimeOffset are indices (15 Hz) */
    TimeOffset=( atoi(&(texttime[0]))*(60*60*1000) ) +
		   ( atoi(&(texttime[3]))*(60*1000) ) +
		   ( atoi(&(texttime[6]))*(1000) );
    continue;
    }
  else if (strcmp(text,"END") == 0)
    {		/* not used in this program */
    i=fscanf(fpt,"%s %s",textdate,texttime);
    continue;
    }
  else		/* assume it is an event(EA) */
    {		/* shimmer data has start-time, end-time, location,
		** helpings, accompaniment, meal-description, activity(s) */
    i=fscanf(fpt,"%s %s %s %s %s %s %s",texttime,texttime2,
	EventLocation[TotalEvents],EventHelpings[TotalEvents],
	EventCompany[TotalEvents],EventFoods[TotalEvents],
	EventActivities[TotalEvents]);
    strcpy(EventTimeStart[TotalEvents],texttime);
    strcpy(EventTimeEnd[TotalEvents],texttime2);
    texttime[2]=texttime[5]=0;	/* null out colons */
    texttime2[2]=texttime2[5]=0;	/* null out colons */
    EventStart[TotalEvents]=( atoi(&(texttime[0]))*(60*60*1000) ) +
			  ( atoi(&(texttime[3]))*(60*1000) ) +
			  ( atoi(&(texttime[6]))*(1000) );
	/* convert from ms since midnight to index since 0 */
	/* the +-.1 at end avoids rounding error converting double to int */
    if ((EventStart[TotalEvents]-TimeOffset) >= 0)
      EventStart[TotalEvents]=(int)((double)(EventStart[TotalEvents]-
		TimeOffset)/(1000.0/15.0)+.1);
    else	/* need to subtract .1 to round to closest integer */
      EventStart[TotalEvents]=(int)((double)(EventStart[TotalEvents]-
		TimeOffset)/(1000.0/15.0)-.1);
    EventEnd[TotalEvents]=( atoi(&(texttime2[0]))*(60*60*1000) ) +
		( atoi(&(texttime2[3]))*(60*1000) ) +
		( atoi(&(texttime2[6]))*(1000) );
	/* convert from ms since midnight to index since 0 */
	/* the +-.1 at end avoids rounding error converting double to int */
    if ((EventEnd[TotalEvents]-TimeOffset) >= 0)
      EventEnd[TotalEvents]=(int)((double)(EventEnd[TotalEvents]-
		TimeOffset)/(1000.0/15.0)+.1);
    else	/* need to subtract .1 to round to closest integer */
      EventEnd[TotalEvents]=(int)((double)(EventEnd[TotalEvents]-
		TimeOffset)/(1000.0/15.0)-.1);
	/* check for crossing midnight, in which case add a day */
    if (TotalEvents>0  &&  EventStart[TotalEvents] < EventStart[TotalEvents-1])
      {
      EventStart[TotalEvents]+=(24*60*60*15);
      EventEnd[TotalEvents]+=(24*60*60*15);
      }
    strcpy(EventName[TotalEvents],text);
    TotalEvents++;
    }
  }
fclose(fpt);

for (i=0; i<TotalEvents; i++)
  {
  printf("%s %d %d %s %s %s %s %s %s %s %s %s\n",
	PID,EventStart[i],EventEnd[i],EventTimeStart[i],EventTimeEnd[i],
	Weekday[weekday],EventName[i],EventLocation[i],EventHelpings[i],
	EventCompany[i],EventFoods[i],EventActivities[i]);
  }

}

