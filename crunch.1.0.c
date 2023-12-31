/*
 *  character-set based brute-forcing algorithm
 *  (c) 2004 by mimayin@aciiid.ath.cx
 *
 *  usage: ./crunch <from-len> <to-len> [charset] [-t [FIXED]@@@@] [-s startblock]
 *
 *  e.g: ./crunch 3 7 abcdef
 *
 *  This example will compute all passwords between 3 and 7 chars
 *  using 'abcdef' as the character set and dump it to stdout.
 *
 *  Options:
 *
 *  -t [FIXED]@@@@  :  allows you to specify a pattern, eg: @@god@@@@ 
 *                     where the only the @'s will change
 *
 *  -s              :  allows you to specify the starting string, eg: 03god22fs
 *
 *
 *  This code can be easily adapted for use in brute-force attacks
 *  against network services or cryptography.
 *
 *  Compiles on: linux, ms visual c++, freebsd
 *
 * https://sourceforge.net/projects/crunch-wordlist/
 * Copyright (C) 2004 by mimayin@aciiid.ath.cx
 * Copyright (C) 2008, 2009, 2010, 2011, 2012 by bofh28@gmail.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE	1
#define FALSE	0

char def_charset[] = "abcdefghijklmnopqrstuvwxyz";

unsigned int inc[128];

int finished(char *block, char *charset, char *templ)
{
	unsigned int i;

	if(templ[0]==0)
	{
		for(i=0;i<strlen(block);i++)
			if(inc[i] < strlen(charset)-1)	return FALSE;
	}
	else
	{
		for(i=0;i<strlen(block);i++)
			if(templ[i]=='@' && (inc[i] < strlen(charset)-1)) return FALSE;
	}

	return TRUE;
}

void increment(char *block, int len, char *charset, char *templ)
{
	int i;

	for(i=strlen(block)-1;i>-1;i--)
	{
		if(templ[0]==0 || templ[i]=='@')
		{
			if(inc[i] < strlen(charset)-1)
			{
				inc[i]++;
				block[i] = charset[inc[i]];
				i=-1;
			}
			else
			{
				block[i] = charset[0];
				inc[i] = 0;
			}
		}
		else
		{
		}
	}
}

void chunk(int start, int end, char *charset, char *templ, char *startblock)
{
	int i,j,k,t;
	char block[128];

	if(end-start <0) return;

	if(templ[0]==0) t=0;
	else		t=1;

	for(i=start;i<=end;i++)
	{
		memset(block,0,sizeof(block));

		for(j=0;j<i;j++)
		{
			if(startblock[0]==0)
			{
				if(t==0)
				{
					block[j] = charset[0];
					inc[j] = 0;
				}
				else
				{
					if(templ[j]=='@')
					{
						block[j] = charset[0];
						inc[j] = 0;
					}
					else			block[j] = templ[j];
				}
			}
			else
			{
				block[j] = startblock[j];

				for(k=0;k<strlen(charset);k++)
					if(block[j]==charset[k]) inc[j] = k;
			}
		}

		printf("%s\n",block);
		while(!finished(block,charset,templ))
		{
			increment(block,i,charset,templ);
			printf("%s\n",block);
		}
	}
}

int main(int argc, char *argv[])
{
	int i,j,flag,min,max;
	char charset[256];
	char templ[256];
	char startblock[256];

	if(argc<3)
	{
		printf("usage: %s <from-len> <to-len> [charset] [-t [FIXED]@@@@] [-s startblock]\n",argv[0]);
		return 0;
	}

        memset(charset,0,sizeof(charset));
        memset(templ,0,sizeof(templ));
        memset(startblock,0,sizeof(startblock));

	/* remove duplicate characters from charset */
	if(argc>3)
	{
		for(i=0;i<strlen(argv[3]);i++)
		{
			flag = 0;
			for(j=0;j<strlen(charset);j++)
				if(argv[3][i] == charset[j]) flag = 1;

			if(flag==0) strncat(charset,&argv[3][i],1);
		}
	}
	else		strncpy(charset,def_charset,strlen(def_charset));

	min = atoi(argv[1]);
	max = atoi(argv[2]);

	for(i=1;i<argc-1;i++)
	{
		if(strncmp(argv[i],"-t",2)==0)
		{
			strncpy(templ,argv[i+1],strlen(argv[i+1]));
		}

		if(strncmp(argv[i],"-s",2)==0)
		{
			if(strlen(argv[i+1]) >min)
			{
				printf("Warning: starting length increased to %d.\n",strlen(argv[i+1]));
				min = strlen(argv[i+1]);
			}
			if(strlen(argv[i+1]) <min)
			{
				printf("Warning: minimum length decreased to %d.\n",strlen(argv[i+1]));
				min = strlen(argv[i+1]);
			}

			strncpy(startblock,argv[i+1],strlen(argv[i+1]));
		}
	}

        chunk(min,max,charset,templ,startblock);

        return 0;
}
