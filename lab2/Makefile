CC=gcc
CFLAGS=-D_POSIX_C_SOURCE=200809L -W -Wall -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic

.PHONY: clean

export CHILD_PATH=./child

all: parent child run

parent: parent.c
	$(CC) $(CFLAGS) -o parent parent.c
    
child: child.c
	$(CC) $(CFLAGS) -o child child.c 

run: parent.c
	./parent text.txt  