CC= gcc
CFLAGS= -std=c99 -Wall -O

all: griddisplay gridserver vehicleclient

griddisplay: griddisplay.c
	${CC} {CFLAGS} -o griddisplay griddisplay.c
	
gridserver: gridserver.c