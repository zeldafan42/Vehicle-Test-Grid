/*
 * gridserver.c
 *
 *  Created on: May 22, 2015
 *      Author: bomwald
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>


int main()
{
	int msgid;



	if( (msgid = msgget(KEY,PERM|IPC_CREAT|IPC_EXCL ))==-1 ){
	/* error handling */
	}

	while(msgrcv(msgid,&msg,sizeof(msg)-sizeof(long),-3, 0)== -1)
	{

	}

	return 0;
}
