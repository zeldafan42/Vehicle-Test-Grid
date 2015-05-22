/*
 * gridserver.c
 *
 *  Created on: May 22, 2015
 *      Author: bomwald
 */
#include <stdio.h>
#include <gridtypes.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>


int getStartingPosition(Message_handshake* serverHandshake, char* field, int fieldX, int fieldY);
void makeMove(Client* client, char action, char* field, int fieldX, int fieldY);

int main()
{
	int msgid;




	if( (msgid = msgget(KEY,PERM|IPC_CREAT|IPC_EXCL ))==-1 )
	{
	/* error handling */
	}



	return 0;
}


void runningServer(int msgid, int fieldX, int fieldY)
{
	Message_move setup1;
	Message_handshake setup2;

	Message_move* clientMessage = &setup1;
	Message_handshake* serverHandshake = &setup2;

	int currentClientNumber;
	Client clients [26];

	char* field = malloc(sizeof(char)*(fieldX+2)*(fieldY+2));



	while(msgrcv(msgid,&clientMessage,sizeof(Message_move)-sizeof(long),-25, 0) == -1)
		{
			currentClientNumber = clientMessage->msgType;
			clients[currentClientNumber]->pid = clientMessage->pid;

			if(clientMessage->action == '0')
			{
				serverHandshake->msgType = (clientMessage->msgType + 26);

				if(getStartingPosition(serverHandshake) == -1)
				{
					/* send SIGTERM because there is no space */
				}
				else
				{
					msgsnd(msgid, serverHandshake, sizeof(Message_handshake)-sizeof(long), serverHandshake->msgType, PERM);
				}
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
	}

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
