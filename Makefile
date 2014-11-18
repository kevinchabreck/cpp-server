server: modular_version.cpp ConnObj.h response.cpp
	g++ -o server -ggdb3 -Wall -Werror -pedantic -std=gnu++11 modular_version.cpp -lpthread
