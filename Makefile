CC=g++
FLAGS= -ggdb3 -Wall -Werror -pedantic -std=gnu++11

build:
	make server
server: server.o ConnObj.o response.o head.o badRequest.o delete.o putResponse.o options.o
	$(CC) $(FLAGS) server.o ConnObj.o response.o head.o badRequest.o delete.o putResponse.o options.o -o server -lpthread

server.o: ConnObj.o response.o putResponse.o head.o badRequest.o delete.o options.o
response.o: response.h ConnObj.o
ConnObj.o: ConnObj.h
putResponse.o: putResponse.h
head.o: head.h ConnObj.o
badRequest.o: badRequest.h ConnObj.o
delete.o: delete.h ConnObj.o
options.o: options.h ConnObj.o

clean:
	rm -f *.o
	rm server
