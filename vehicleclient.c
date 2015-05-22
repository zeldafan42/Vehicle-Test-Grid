/*
 * vehicleclient.c
 *
 *  Created on: May 22, 2015
 *      Author: bomwald
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <gridtypes.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main(int argc, char* argv[])
{
	char symbol;
	int msgid;
	Message_handshake handshake;
	Message_move move;
	pid_t pid = getpid();
	int typeId;


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

	symbol = argv[1][0];
	typeId = symbol - 'A';

	move->msgType = typeId;
	move->action = REGISTER;
	move->pid = pid;

	if(msgget(KEY,PERM) == NULL)
	{
		fprintf(stderr, "Could not get message queue, maybe server is not yet running?");
		return -1;
	}

	if (msgsnd(msgid,&move,sizeof(Message_move)-sizeof(long), 0) == -1)
   {
		 /* error handling */
		 fprintf(stderr,"%s: Can't send message\n",argv[0]);
		 return -1;
   }

	if (msgrcv())



	return 0;
}

