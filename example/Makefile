CC=gcc

CFLAGS=-g -Wall
INC_PATH=-I ../include
LIB_PATH=-L ../
LIBS=-lnsprx -lnspr4

examples=channel_example

all:$(examples)

channel_example:channel_example.c
	$(CC) $(CFLAGS) $(INC_PATH) $(LIB_PATH) $^ -o $@ $(LIBS)

clean:
	rm $(examples)
