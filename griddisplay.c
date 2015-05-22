/*
 * griddisplay.c
 *
 *  Created on: May 22, 2015
 *      Author: bomwald
 */

#include "gridtypes.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


void signalHandler(int sig);

FILE * fp;

int main(int argc, char* argv[])
{
	char msgBuf[MAX_SIZE];
	char * msg;

	signal(SIGINT, signalHandler);
	signal(SIGHUP, signalHandler);
	signal(SIGQUIT, signalHandler);

	if((fp = fopen(PIPE_NAME, "r")) == NULL)
	{
		fprintf(stderr, "%s: Could not access server pipe", argv[0]);
		return -1;
	}
	while(1)
	{
		msg = fgets(msgBuf, MAX_SIZE-1, fp);
		if(msg == NULL)
		{
			signalHandler(SIGPIPE);
		}
		printf("%s",msg);
	}

	return 0;
}

void signalHandler(int sig)
{
	if(fp != NULL)
	{
		if(fclose(fp) == EOF)
		{
			fprintf(stderr, "griddisplay: Could not close pipe.\n");
		}
	}
	exit(0);
}
