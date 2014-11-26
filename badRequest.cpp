#include <iostream>
#include "badRequest.h"
#include <stdio.h>
#include <stdlib.h>
#include "ConnObj.h"

void  badRequest(ConnObj* conn_state){
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

  header+="HTTP/1.1 404 Bad Request\r\nDate: "+ dateTime +"\r\nServer: tinyserver.colab.duke.edu\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);
  
  FILE* html_file = fopen("www/badReq.html", "r");
  while((numBytes = fread(html,1,8000,html_file)) > 0){
      send(conn_state->response_socket,html,numBytes,0);
    }
}   
