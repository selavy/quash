CC = gcc
DEBUG = -DDEBUG -g
CFLAGS = -Wall -Werror -ansi -pedantic $(DEBUG)
OBJS = main.o parse_token.o jobs.o set.o change_dir.o execute_command.o general.o

quash: $(OBJS)
	$(CC) $(CFLAGS) -o quash $(OBJS)
main.o: main.c
	$(CC) $(CFLAGS) -c main.c
parse_token.o: parse_token.h parse_token.c
	$(CC) $(CFLAGS) -c parse_token.c
jobs.o: jobs.h jobs.c
	$(CC) $(CFLAGS) -c jobs.c
set.o: set.h set.c
	$(CC) $(CFLAGS) -c set.c
change_dir.o: change_dir.h change_dir.c
	$(CC) $(CFLAGS) -c change_dir.c
execute_command.o: execute_command.h execute_command.c
	$(CC) $(CFLAGS) -c execute_command.c
general.o: general.h general.c
	$(CC) $(CFLAGS) -c general.c
.PHONY:clean
clean:
	rm -rf *.o quash *~