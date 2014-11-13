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
#include "response.h"
#include <stdlib.h>
#include <zlib.h>


ssize_t simpleResponse(int responseSocket){
  //Head and HTML buffers
  std::string header;
  char html[8000];
  int numBytes = 0;

  //Time Struct
  time_t ping;
  struct tm* currentTime;
  char timeBuffer[80];
  
  time(&ping);
  currentTime = localtime(&ping);
  strftime(timeBuffer,80,"%a, %d %h %G %T %z",currentTime);
  std::string dateTime = (timeBuffer);



  //File
  FILE* html_file = fopen("in.html", "r");
  if(html_file  == NULL){
    //BAD REQUEST
  }
    while((numBytes = fread(html,1,8000,html_file)) > 0){
      header+="HTTP1.1 200 OK\r\nDate: "+ dateTime +"\r\nServer: tinyserver.colab.duke.edu\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(numBytes) + "\r\n\r\n";
      send(responseSocket,header.c_str(),header.length(),0);
      send(responseSocket,html,numBytes,0);
    }
  
    return 1;
}

