/*
 * gridtypes.h
 *
 *  Created on: May 22, 2015
 *      Author: bomwald
 */

#ifndef GRIDTYPES_H_
#define GRIDTYPES_H_

#define _GNU_SOURCE
#include <sys/types.h>
#define PIPE_NAME "display.pipe"
#define KEY 13371337
#define PERM 0777
#define MAX_SIZE 4096
#define NORTH 'N'
#define EAST 'E'
#define SOUTH 'S'
#define WEST 'W'
#define TERMINATE 'T'
#define REGISTER '0'


typedef struct
{
	long mType;
	pid_t pid;
	char action;
} Message_move;


typedef struct
{
	long mType;
	int startX;
	int startY;
} Message_handshake;


typedef struct
{
	pid_t pid;
	char vehicleName;
	int x;
	int y;
} Client;


#endif /* GRIDTYPES_H_ */
