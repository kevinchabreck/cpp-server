CC=g++
FLAGS= -ggdb3 -Wall -Werror -pedantic -std=gnu++11

build:
	make server

server: server.o ConnObj.o badRequest.o response.o putResponse.o
	$(CC) $(FLAGS) server.o ConnObj.o badRequest.o response.o putResponse.o -o server -lpthread

server.o: ConnObj.o response.o
response.o: response.h ConnObj.o
ConnObj.o: ConnObj.h
badRequest.o: badRequest.h
putResponse.o: putResponse.h

clean:
	rm -f *.o
	rm server
