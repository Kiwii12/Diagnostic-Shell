#-----------------------------------------------------------------------

CC=g++

CFLAGS=-c -g -Wall -std=c++11

all: dsh

dsh: dsh1
	${CC} -g -Wall -std=c++11 dsh.o -o dsh

dsh1:
	${CC} ${CFLAGS} dsh.cpp -o dsh.o

clean:
	rm *o dsh

#-----------------------------------------------------------------------git