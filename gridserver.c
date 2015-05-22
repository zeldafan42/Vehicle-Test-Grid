/*
 * gridserver.c
 *
 *  Created on: May 22, 2015
 *      Author: bomwald
 */
#include <stdio.h>
#include "gridtypes.h"
#include <signal.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

void printUsage();
void signalHandler(int sig);
void runningServer(int msgid, int fieldX, int fieldY);

int getStartingPosition(Message_handshake* serverHandshake, int fieldX, int fieldY);
int makeMove(Client* client, char action, int fieldX, int fieldY);
void collisionCheck(Client* crashingClient, int fieldX, int fieldY);
void killClient(Client* client, fieldX, fieldY);

global Client** clients = (Client**) malloc(sizeof(Client)*26);
global char* field;

int main(int argc, char* argv[])
{
	int c = 0;
	int errorX = 0;
	int errorY = 0;
	int fieldX = 0;
	int fieldY = 0;

	while( (c = getopt(argc, argv, "x:y:")) != EOF )
	{
		switch(c)
		{
		case 'x':	fieldX = optarg;

					if(errorX)
					{
						fprinft("Use the -x  argument only once!\n");
						printUsage();
					}

					errorX = 1;

					break;

		case 'y':	fieldY = optarg;

					if(errorY)
					{
						fprinft("Use the -y  argument only once!\n");
						printUsage();
					}

					errorY = 1;

					break;

		case '?':	printUsage();
					break;
		}

	}

	if(fieldX<1 || fieldY<1) /* The field should have a height and width which makes sense not just any number */
	{
		printUsage();
	}
	else
	{

		int msgid;


		if( (msgid = msgget(KEY,PERM|IPC_CREAT|IPC_EXCL ))==-1 )
		{
		/* error handling */
		}

		signal(SIGTERM, signalHandler);
		signal(SIGINT, signalHandler);
		signal(SIGQUIT, signalHandler);
		signal(SIGHUP, signalHandler);
		signal(SIGKILL, signalHandler); /* As if that would do anything :D */

		runningServer(msgid,fieldX+2,fieldY+2);

		return 0;
	}
}

void printUsage()
{
	fprintf(stderr, "Usage: gridserver -x INT>0 -y INT>0\n");
	exit(1);
}

void signalhandler(int sig)
{
	free(field);
	free(clients);

	exit(0);
}


void runningServer(int msgid, int fieldX, int fieldY)
{
	Message_move setup1;
	Message_handshake setup2;

	Message_move* clientMessage = &setup1;
	Message_handshake* serverHandshake = &setup2;

	int currentClientNumber;

	field = (char*) malloc(sizeof(char)*fieldX*fieldY);

	int i = 0;

	for(i=0; i<26; i++)
	{
		clients[i]->vehicleName = ' ';
	}




	while(msgrcv(msgid,&clientMessage,sizeof(Message_move)-sizeof(long),-25, 0) == -1)
		{
			currentClientNumber = clientMessage->msgType;

			if(clients[currentClientNumber]->vehicleName == ' ')
			{
				clients[currentClientNumber]->pid = clientMessage->pid;


				if(clientMessage->action == '0') /*if the client is requesting a starting position */
				{
					serverHandshake->msgType = (clientMessage->msgType + 26);

					if(getStartingPosition(serverHandshake) == -1) /* if the field is full */
					{
						serverHandshake->startX = -1; /* error signal for the client that the field is full */
						serverHandshake->startY = -1;

						msgsnd(msgid, serverHandshake, sizeof(Message_handshake)-sizeof(long), serverHandshake->msgType, PERM);
					}
					else  /* if there is an available starting position */
					{
						msgsnd(msgid, serverHandshake, sizeof(Message_handshake)-sizeof(long), serverHandshake->msgType, PERM);
					}
				}
				else /* if the client just wants to move */
				{
					makeMove(clients[currentClientNumber], clientMessage->action, fieldX, fieldY);

				}

			}
			else /* if the vehicle is already on the field */
			{
				serverHandshake->msgType = (clientMessage->msgType + 26);

				serverHandshake->startX = -2; /* error signal for the client that the vehicle is already on the field */
				serverHandshake->startY = -2; /* As only the new one is listening for a handshake the first one will not be affected at all */
			}

		}
}


void makeMove(Client* client, char action, int fieldX, int fieldY)
{
	int currentX = client->x;
	int currentY = client->y;

	int targetX = currentX;
	int targetY = currentY;

	switch(action)
	{
		case 'N': 	targetY--;
					break;

		case 'E': 	targetX++;
					break;

		case 'S': 	targetY++;
					break;

		case 'W': 	targetX--;
					break;

		case 'T':   kill(client);
					break;

		default:	break;
	}

	if(1 >= targetX  || targetX >= fieldX || 1 >= targetY  || targetY >= fieldY)
	{
		killClient(client, fieldX, fieldY);
	}

	if(field[targetY*fieldX + targetX] == ' ') /* is the space where the vehicle is moving to free? */
	{
		field[targetY*fieldX + targetX] = client->vehicleName;
		field[currentY*fieldX + currentX] = ' ';
		client->x = targetX;
		client->y = targetY;

		return;
	}
	else /* if there is already a vehicle on the field where this one is moving to */
	{
		client->x = targetX;
		client->y = targetY;

		collisionCheck(client, fieldX, fieldY);
	}

}

void collisionCheck(Client* crashingClient, int fieldX, int fieldY)
{
	int i = 0;
	int x = 0;
	int y = 0;

	for(i=0; i<26; i++)
	{
		if(clients[i]->x == crashingClient->x && clients[i] ==  crashingClient->y)
		{
			killClient(clients[i]);
		}
	}

}

void killClient(Client* client, fieldX, fieldY)
{
	client->vehicleName = ' ';
	field[client->y*fieldX + client->x] = client->vehicleName;

	kill(client->pid, SIGTERM);
}


int getStartingPosition(Message_handshake* serverHandshake, char* field, int fieldX, int fieldY)
{
	int x = 0;
	int y = 0;

	for(y=1; y < (fieldY-1); y++)
	{
		for(x=1; x < (fieldX-1); x++)
		{
			if(field[y*fieldX + x] == ' ')
			{
				serverHandshake->startX = x;
				serverHandshake->startY = y;
				return 0;
			}
		}
	}

	return -1;

}
