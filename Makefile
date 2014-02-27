CC = gcc
CFLAGS = -Wall -Werror -g -ansi -pedantic

OBJS = main.o

quash: $(OBJS)
	$(CC) $(CFLAGS) -o quash $(OBJS)
main.o: main.c
	$(CC) $(CFLAGS) -c main.c
.PHONY:clean
clean:
	rm -rf *.o quash