CC=g++
FLAGS= -ggdb3 -Wall -Werror -pedantic -std=gnu++11

build:
	make server

server: server.o connection.o get.o head.o common.o delete.o put.o options.o post.o
	$(CC) $(FLAGS) server.o connection.o get.o head.o common.o delete.o put.o options.o post.o -o server -lpthread -lz

server.o: connection.o get.o put.o head.o delete.o options.o post.o
get.o: get.h connection.o common.o
connection.o: connection.h
put.o: put.h connection.o common.o
head.o: head.h connection.o
common.o: common.h connection.o
delete.o: delete.h connection.o common.o
options.o: options.h connection.o
post.o: post.h connection.o common.o

clean:
	rm -f *.o
	rm server
