/*
 * gridserver.c
 *
 *  Created on: May 22, 2015
 *      Author: bomwald
 */
#include <stdio.h>
#include "gridtypes.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

void runningServer(int msgid,Client* clients[26], int fieldX, int fieldY);

int getStartingPosition(Message_handshake* serverHandshake, char* field, int fieldX, int fieldY);
int makeMove(Client* client, char action, char* field, int fieldX, int fieldY);

int main()
{
	Client* clients [26] = malloc(sizeof(Client)*26);

	int msgid;


	if( (msgid = msgget(KEY,PERM|IPC_CREAT|IPC_EXCL ))==-1 )
	{
	/* error handling */
	}



	return 0;
}


void runningServer(int msgid,Client* clients[26], int fieldX, int fieldY)
{
	Message_move setup1;
	Message_handshake setup2;

	Message_move* clientMessage = &setup1;
	Message_handshake* serverHandshake = &setup2;

	int currentClientNumber;

	char* field = malloc(sizeof(char)*(fieldX+2)*(fieldY+2));

	int crash = 0;
	int i = 0;
	int j = 0;

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
					makeMove(clients[currentClientNumber], clientMessage->action, char* field, fieldX, fieldY);

				}

			}
			else /* if the vehicle is already on the field */
			{
				serverHandshake->msgType = (clientMessage->msgType + 26);

				serverHandshake->startX = -2; /* error signal for the client that the vehicle is already on the field */
				serverHandshake->startY = -2;
			}

		}
}


void makeMove(Client* client, char action, char* field, int fieldX, int fieldY)
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
		kill(client);
	}

	if(field[targetY*fieldX + targetX] == ' ')
	{
		field[targetY*fieldX + targetX] = client->vehicleName;
		field[currentY*fieldX + currentX] = ' ';
		client->x = targetX;
		client->y = targetY;

		return;
	}
	else
	{
		client->x = targetX;
		client->y = targetY;

		collisionCheck(client);
	}

}

void collisionCheck(Client* client)
{
	int i = 0;
	int x = 0;
	int y = 0;




}


int getStartingPosition(Message_handshake* serverHandshake, char* field, int fieldX, int fieldY)
{
	int x = 0;
	int y = 0;

	for(x=1; x < (fieldX-1); x++)
	{
		for(y=1; y < (fieldY-1); y++)
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
