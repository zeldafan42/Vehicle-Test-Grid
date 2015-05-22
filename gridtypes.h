/*
 * gridtypes.h
 *
 *  Created on: May 22, 2015
 *      Author: bomwald
 */

#ifndef GRIDTYPES_H_
#define GRIDTYPES_H_

#define PIPE_NAME "display.pipe"
#define KEY 13371337
#define PERM 0660
#define MAX_SIZE 4096


typedef struct
{
	long msgType;
	int pid;
	char action;
} Message_move;


typedef struct
{
	long msgType;
	int startX;
	int startY;
} Message_handshake;


typedef struct
{
	int pid;
	int x;
	int y;
} Client;


#endif /* GRIDTYPES_H_ */
