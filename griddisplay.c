/*
 * griddisplay.c
 *
 *  Created on: May 22, 2015
 *      Author: bomwald
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "gridtypes.h"

void signalHandler(int sig);

int main(int argc, char* argv[])
{
	FILE * fp;
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
		printf("%s",msg);
	}

	return 0;
}

void signalHandler(int sig)
{
	exit(0);
}
