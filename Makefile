server: socket_setup.cpp response.cpp
	g++ -o server -ggdb3 -Wall -Werror -pedantic -std=gnu++11 socket_setup.cpp response.cpp
