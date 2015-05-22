CC= gcc
CFLAGS= -std=c99 -Wall -O

all: griddisplay gridserver vehicleclient

griddisplay: griddisplay.c gridtypes.h
	${CC} ${CFLAGS} -o griddisplay griddisplay.c
	
gridserver: gridserver.c gridtypes.h
	${CC} ${CFLAGS} -o gridserver gridserver.c
	
vehicleclient: vehicleclient.c gridtypes.h
	${CC} ${CFLAGS} -o vehicleclient vehicleclient.c
	
clean:
	rm -f griddisplay gridserver vehicleclient