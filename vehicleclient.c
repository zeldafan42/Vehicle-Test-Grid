/*
 * vehicleclient.c
 *
 *  Created on: May 22, 2015
 *      Author: bomwald
 */

#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		fprintf(stderr, "Usage: %s SYMBOL(A-Z)", argv[0]);
		return -1;
	}

	if(strlen(argv[1]) != 1)
	{
		fprintf(stderr, "Only one symbol allowed");
		return -1;
	}

	if(argv[1][0] > 'Z' || argv[1][0] <'A')
	{
		fprintf(stderr, "Only A-Z allowed");
		return -1;
	}

	return 0;
}

