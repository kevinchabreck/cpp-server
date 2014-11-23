CC=g++
FLAGS= -ggdb3 -Wall -Werror -pedantic -std=gnu++11 -lpthread

build:
	make server

server: server.o ConnObj.o response.o
	$(CC) $(FLAGS) server.o ConnObj.o response.o -o server

server.o: ConnObj.o response.o
response.o: response.h ConnObj.o
ConnObj.o: ConnObj.h

clean:
	rm -f *.o
	rm server