#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <cstdlib>
#include <exception>
#include <assert.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <time.h>
#include <zlib.h>

ssize_t simpleResponse(int responseSocket, char* fileName);


#endif


