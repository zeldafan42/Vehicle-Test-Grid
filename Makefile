CC= gcc
CFLAGS= -std=c99 -Wall -O

all: griddisplay gridserver vehicleclient

griddisplay: griddisplay.c
	${CC} ${CFLAGS} -o griddisplay griddisplay.c
	
gridserver: gridserver.c
	${CC} ${CFLAGS} -o gridserver gridserver.c
	
vehicleclient: vehicleclient.c
	${CC} ${CFLAGS} -o vehicleclient vehicleclient.c
	
clean:
	rm -f griddisplay gridserver vehicleclient