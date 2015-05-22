/*
 * gridserver.c
 *
 *  Created on: May 22, 2015
 *      Author: bomwald
 */
#include "gridtypes.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>

void printUsage();
void signalHandler(int sig);
void runningServer(int fieldX, int fieldY);

int getStartingPosition(Client* client, Message_handshake* serverHandshake, int fieldX, int fieldY);
void makeMove(Client* client, char action, int fieldX, int fieldY);
void collisionCheck(Client* crashingClient, int fieldX, int fieldY);
void killClient(Client* client, int fieldX, int fieldY);
void printToPipe(int fieldX, int fieldY);

Client** clients;
char* field;
int msgid;
FILE * fp;

int main(int argc, char* argv[])
{
	msgid = -1;
	int c = 0;
	int errorX = 0;
	int errorY = 0;
	int fieldX = 0;
	int fieldY = 0;

	while( (c = getopt(argc, argv, "x:y:")) != EOF )
	{
		switch(c)
		{
		case 'x':	fieldX = (int) strtol(optarg, NULL, 0);

					if(errorX)
					{
						fprintf(stderr,"Use the -x  argument only once!\n");
						printUsage();
					}

					errorX = 1;

					break;

		case 'y':	fieldY = (int) strtol(optarg, NULL, 0);

					if(errorY)
					{
						fprintf(stderr,"Use the -y  argument only once!\n");
						printUsage();
					}

					errorY = 1;

					break;

		case '?':	printUsage();
					break;

		default:	assert(0);
					break;
		}

	}

	if(fieldX<1 || fieldY<1) /* The field should have a height and width which makes sense not just any number */
	{
		printUsage();
		return -1;
	}
	else
	{

		signal(SIGTERM, signalHandler);
		signal(SIGINT, signalHandler);
		signal(SIGQUIT, signalHandler);
		signal(SIGHUP, signalHandler);
		signal(SIGPIPE, signalHandler);
		//signal(SIGKILL, signalHandler); /* As if that would do anything :D */

		runningServer(fieldX+2,fieldY+2);
		signalHandler(1);

		return 0;
	}
}

void printUsage()
{
	fprintf(stderr, "Usage: gridserver -x INT>0 -y INT>0\n");
	exit(1);
}

void signalHandler(int sig)
{
	int i = 0;
	if(field != NULL)
	{
		free(field);
		field = NULL;
	}

	if(clients != NULL)
	{
		for(i=0; i<26; i++)
		{
			if(clients[i] != NULL)
			{
				if(clients[i]->pid != -1)
				{
					kill(clients[i]->pid, SIGTERM);
				}
				free(clients[i]);
				clients[i] = NULL;
			}
		}
		free(clients);
		clients = NULL;
	}

	if(msgid != -1)
	{
		if(msgctl(msgid, IPC_RMID, NULL) == -1)
		{
			fprintf(stderr, "gridserver: Could not close message queue.\n");
		}
	}

	if(fp != NULL && sig != SIGPIPE)
	{
		if(fclose(fp) == EOF)
		{
			fprintf(stderr, "gridserver: Could not close pipe.\n");
		}
	}

	if(remove(PIPE_NAME) == -1)
	{
		fprintf(stderr, "gridserver: Could not remove pipe. Please remove manually\n");
	}


	exit(0);
}


void runningServer(int fieldX, int fieldY)
{
	Message_move setup1;
	Message_handshake setup2;

	Message_move* clientMessage = &setup1;
	Message_handshake* serverHandshake = &setup2;

	int x = 0;
	int y = 0;

	if(mkfifo(PIPE_NAME, PERM) == -1)
	{
		fprintf(stderr, "gridserver: Could not create pipe\n");
		return;
	}

	int currentClientNumber;

	if(fieldX*fieldY + fieldY + 1 >= MAX_SIZE)
	{
		fprintf(stderr, "gridserver: Field size is ridiculous\n");
		return;
	}

	field = (char*) malloc(sizeof(char)*fieldX*fieldY);


	for(y=0; y < fieldY; y++)
	{
		for(x=0; x < fieldX; x++)
		{
			if(y == 0 || y == fieldY-1 || x == 0 || x == fieldX-1)
			{
				field[y*fieldX + x] = '#';
			}
			else
			{
				field[y*fieldX + x] = ' ';
			}
		}
	}

	int i = 0;

	clients = (Client**) malloc(sizeof(Client*)*26);

	for(i=0; i<26; i++)
	{
		clients[i] = (Client*) malloc(sizeof(Client));
		clients[i]->vehicleName = ' ';
		clients[i]->pid = -1;
		clients[i]->x = -1;
		clients[i]->y = -1;

	}

	if( (msgid = msgget(KEY,PERM|IPC_CREAT|IPC_EXCL ))==-1 )
	{
		fprintf(stderr,"gridserver: Can't create message queue\n");
		return;
	}

	if((fp = fopen(PIPE_NAME, "w")) == NULL)
	{
		fprintf(stderr, "gridserver: Could not open pipe\n");
		return;
	}

	while(1)
	{
		if(msgrcv(msgid,clientMessage,sizeof(Message_move)-sizeof(long),-26, 0) == -1)
		{
			fprintf(stderr,"gridserver: Can't receive message\n");
			return;
		}

		currentClientNumber = clientMessage->mType -1;
		//printf("%d\n",currentClientNumber);
		//fflush(stdout);

		if(clients[currentClientNumber]->vehicleName == ' ')
		{
			clients[currentClientNumber]->pid = clientMessage->pid;

			clients[currentClientNumber]->vehicleName = clientMessage->mType + 'A' - 1;

			serverHandshake->mType = (clientMessage->mType + 26);

			if(getStartingPosition(clients[currentClientNumber],serverHandshake, fieldX, fieldY) == -1) /* if the field is full */
			{
				serverHandshake->startX = -1; /* error signal for the client that the field is full */
				serverHandshake->startY = -1;
				clients[currentClientNumber]->vehicleName = ' ';

				msgsnd(msgid, serverHandshake, sizeof(Message_handshake)-sizeof(long), 0);
			}
			else  /* if there is an available starting position */
			{
				msgsnd(msgid, serverHandshake, sizeof(Message_handshake)-sizeof(long), 0);
				printToPipe(fieldX, fieldY);
			}

		}
		else /* if the vehicle is already on the field */
		{
			if(clientMessage->action == '0') /*if the client is requesting a starting position */
			{
				serverHandshake->mType = (clientMessage->mType + 26);

				serverHandshake->startX = -2; /* error signal for the client that the vehicle is already on the field */
				serverHandshake->startY = -2; /* As only the new one is listening for a handshake the first one will not be affected at all */

				msgsnd(msgid, serverHandshake, sizeof(Message_handshake)-sizeof(long), 0);
			}
			else /* if the client just wants to move */
			{
				makeMove(clients[currentClientNumber], clientMessage->action, fieldX, fieldY);
				printToPipe(fieldX, fieldY);
			}
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

		case 'T':   killClient(client, fieldX, fieldY);
					break;

		default:	break;
	}

	if(targetX < 1 || targetX >= fieldX || targetY < 1  || targetY >= fieldY)
	{
		killClient(client, fieldX, fieldY);
	}
	else if(field[targetY*fieldX + targetX] == ' ') /* is the space where the vehicle is moving to free? */
	{
		field[targetY*fieldX + targetX] = client->vehicleName;
		field[currentY*fieldX + currentX] = ' ';
		client->x = targetX;
		client->y = targetY;

		return;
	}
	else /* if there is already a vehicle on the field where this one is moving to */
	{
		field[currentY*fieldX + currentX] = ' ';
		client->x = targetX;
		client->y = targetY;

		collisionCheck(client, fieldX, fieldY);
	}

}

void collisionCheck(Client* crashingClient, int fieldX, int fieldY)
{
	int i = 0;
	int crashX = crashingClient->x;
	int crashY = crashingClient->y;

	for(i=0; i<26; i++)
	{
		if((clients[i]->x == crashX) && (clients[i]->y == crashY ))
		{
			//printf("Killing client: %d\n",i);
			//fflush(stdout);
			killClient(clients[i], fieldX, fieldY);
		}
	}

}

void killClient(Client* client, int fieldX, int fieldY)
{
	if(client->x >= 0 && client->y >= 0 && client->x <= fieldX && client->y <= fieldY)
	{
		if(field[client->y*fieldX + client->x] >= 'A' || field[client->y*fieldX + client->x] <= 'Z')
		{
			field[client->y*fieldX + client->x] = ' ';
			//printf("Killing field %d,%d,%d\n",client->x,client->y, client->pid);
			//fflush(stdout);
		}

		if(client->pid != -1)
		{
			kill(client->pid, SIGTERM);
		}
		client->pid = -1;
		client->x = -1;
		client->y = -1;
		client->vehicleName = ' ';
	}
}


int getStartingPosition(Client* client, Message_handshake* serverHandshake, int fieldX, int fieldY)
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

				field[y*fieldX + x] = client->vehicleName;
				client->x = x;
				client->y = y;
				return 0;
			}
		}
	}

	return -1;

}

void printToPipe(int fieldX, int fieldY)
{
	int x = 0;
	int y = 0;
	int i = 0;
	char buf[MAX_SIZE];
	memset(buf, '\0',MAX_SIZE);

	for(y=0; y < fieldY; y++)
	{
		for(x=0; x < fieldX; x++)
		{
			buf[i] = field[y*fieldX + x];
			i++;
		}
		buf[i] = '\n';
		i++;
	}
	fprintf(fp,"%s",buf);
	fflush(fp);
}
