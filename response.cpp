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


void sendContentLength(int length, int responseSocket){
  char firstSegment[] = "Content-Length: ";
  char secondSegment[] = "\r\n\r\n";
  char buffer[65535];
 
  sprintf(buffer,"%d",length);

  send(responseSocket,firstSegment,strlen(firstSegment),MSG_CONFIRM);
  send(responseSocket,buffer,strlen(buffer),MSG_CONFIRM);
  send(responseSocket,secondSegment,strlen(secondSegment),MSG_CONFIRM);
}


void sendDate(int responseSocket){
  char buffer[1000];
  time_t ping = time(0);
  struct tm now = *gmtime(&ping);
  strftime(buffer,sizeof(buffer), "%a, %d %b %Y %H:%M:%S %Z \r\n", &now);
  send(responseSocket, "Date: ", strlen("Date: "), MSG_CONFIRM);
  send(responseSocket,buffer,strlen(buffer),MSG_CONFIRM);
}


void sendLanguage(int responseSocket){
  char buffer[] = "Content-Language: en-us\r\n";
  send(responseSocket,buffer,strlen(buffer),MSG_CONFIRM);
}

void sendEncoding(int responseSocket){
  char buffer[] = "Content-Encoding: gzip\r\n";
  send(responseSocket,buffer,strlen(buffer),MSG_CONFIRM);
}


void sendStatus(int responseSocket, int code){
  char* buffer;

  switch(code){
  case 200:
    buffer = (char*)"HTTP1.1 200 OK\r\n";
    send(responseSocket,buffer,strlen(buffer),MSG_CONFIRM);
    break;
  default:
    buffer =(char*) "HTTP1.1 202 Accepted\r\n";
    send(responseSocket,buffer,strlen(buffer),MSG_CONFIRM);
   
    break; // WHAT IS THE DEFAULT CASE?
  }
}


ssize_t simpleResponse(int responseSocket, char* fileName){
  char contentType[] = "Content-Type: text/html\r\n";
  char* html [1000];
  int octTotal;


  FILE* htmlFile = fopen(fileName, "r");
  if(htmlFile == NULL){
    char buffer[] = "HTTP1.1 404 Not Found\r\n";
    send(responseSocket,buffer,strlen(buffer),MSG_CONFIRM);
    return (-1);
  }
  

    char* htmlLine = NULL;
  size_t htmlSize;


  //Get HTML Content
  int count = 0;

  while(getline(&htmlLine,&htmlSize,htmlFile) != -1){
    html[count] = htmlLine;
    octTotal += strlen(htmlLine);
    count++;
    htmlLine = NULL;
  }

  

  //Send Response
  sendStatus(responseSocket,202);
  sendDate(responseSocket);
  //  sendEncoding(responseSocket);
  sendLanguage(responseSocket);
  send(responseSocket, contentType,strlen(contentType), MSG_CONFIRM);
  sendContentLength(octTotal,responseSocket);
  
  for(int i = 0; i < count; i++)
    send(responseSocket,html[i],strlen(html[i]),MSG_CONFIRM);
  return 1;
}

