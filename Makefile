CC=g++
FLAGS= -ggdb3 -Wall -Werror -pedantic -std=gnu++11

build:
	make server

server: server.o connection.o get.o head.o badRequest.o delete.o put.o options.o
	$(CC) $(FLAGS) server.o connection.o get.o head.o badRequest.o delete.o put.o options.o -o server -lpthread

server.o: connection.o get.o put.o head.o badRequest.o delete.o options.o
get.o: get.h badRequest.o
connection.o: connection.h
put.o: put.h
head.o: head.h connection.o
badRequest.o: badRequest.h connection.o
delete.o: delete.h connection.o
options.o: options.h connection.o

clean:
	rm -f *.o
	rm server
