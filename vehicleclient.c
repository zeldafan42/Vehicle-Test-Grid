/*
 * vehicleclient.c
 *
 *  Created on: May 22, 2015
 *      Author: bomwald
 */

#include "gridtypes.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

void signalHandler(int sig);

int main(int argc, char* argv[])
{
	char symbol;
	int msgid;
	Message_handshake handshake;
	Message_move move;
	pid_t pid = getpid();
	int typeId;
	char nextMove;

	signal(SIGINT, signalHandler);
	signal(SIGHUP, signalHandler);
	signal(SIGQUIT, signalHandler);
	signal(SIGTERM, signalHandler);

	if(argc != 2)
	{
		fprintf(stderr, "Usage: %s SYMBOL(A-Z)", argv[0]);
		return -1;
	}

	if(strlen(argv[1]) != 1)
	{
		fprintf(stderr, "%s: Only one symbol allowed", argv[0]);
		return -1;
	}

	if(argv[1][0] > 'Z' || argv[1][0] <'A')
	{
		fprintf(stderr, "%s: Only A-Z allowed", argv[0]);
		return -1;
	}

	symbol = argv[1][0];
	typeId = symbol - 'A';

	move.msgType = typeId;
	move.action = REGISTER;
	move.pid = pid;

	if((msgid = msgget(KEY,PERM)) == -1)
	{
		fprintf(stderr, "%s: Could not get message queue, maybe server is not yet running?", argv[0]);
		return -1;
	}

	if (msgsnd(msgid,&move,sizeof(Message_move)-sizeof(long), 0) == -1)
   {
		 /* error handling */
		 fprintf(stderr,"%s: Can't send message\n",argv[0]);
		 return -1;
   }

	if (msgrcv(msgid,&handshake, sizeof(Message_handshake)-sizeof(long),typeId+26, 0) == -1)
	{
		fprintf(stderr,"%s: Can't receive message\n",argv[0]);
		return -1;
	}

	if(handshake.startX<0)
	{
		fprintf(stderr,"%s: Registration FAILED\n",argv[0]);
		return -1;
	}
	else
	{
		printf("Registration OK. Start position: %d, %d",handshake.startX,handshake.startY);
	}

	printf("Enter Move: ");

	while(1)
	{
		nextMove = getchar();
		if((nextMove == NORTH) || (nextMove == EAST) || (nextMove == SOUTH) || (nextMove == WEST) || (nextMove == TERMINATE))
		{
			move.action = nextMove;
			if (msgsnd(msgid,&move,sizeof(Message_move)-sizeof(long), 0) == -1)
			{
				 /* error handling */
				 fprintf(stderr,"%s: Can't send message\n",argv[0]);
				 return -1;
			}
		}
	}

	return 0;
}

void signalHandler(int sig)
{
	printf("Vehicle has been eliminated.\n");
	exit(0);
}
