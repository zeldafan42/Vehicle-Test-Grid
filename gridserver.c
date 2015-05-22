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


void getStartingPosition(Message_handshake* serverHandshake);
void makeMove(Client* client, char action);

int main()
{
	int msgid;
	int currentClientNumber;
	Client clients [26];


	Message_move clientMessage;
	Message_handshake serverHandshake;



	if( (msgid = msgget(KEY,PERM|IPC_CREAT|IPC_EXCL ))==-1 ){
	/* error handling */
	}

	while(msgrcv(msgid,&clientMessage,sizeof(msg)-sizeof(long),-25, 0) == -1)
	{
		currentClientNumber = clientMessage->msgType;
		clients[currentClientNumber]->pid = clientMessage->pid;

		if(clientMessage->action == '0')
		{
			serverHandshake->msgType = (clientMessage->msgType + 26);
			getStartingPosition(&serverHandshake);
			msgsnd(msgid,)
		}

	}

	return 0;
}


