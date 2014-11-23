#include <iostream>
#include "response.h"
#include <stdio.h>
#include <stdlib.h>

ssize_t putResponse(Request* req, ConnObj* conn_state){
  return 0;
}

ssize_t getResponse(Request* req, ConnObj* conn_state){
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

  req->request_URI.erase(0,1);

  //File
  FILE* html_file = fopen(req->request_URI.c_str(), "r");
  if(html_file  == NULL){
    //BAD REQUEST
    header+="HTTP/1.1 404 Bad Request\r\nDate: "+ dateTime +"\r\nServer: tinyserver.colab.duke.edu\r\n\r\n";
    send(conn_state->response_socket,header.c_str(),header.length(),0);
    std::cout << "Bad Request\n";
    FILE* html_file = fopen("www/badReq.html", "r");
    while((numBytes = fread(html,1,8000,html_file)) > 0){
      send(conn_state->response_socket,html,numBytes,0);
    }
    return 0;
  }

  header+="HTTP/1.1 200 OK\r\nDate: "+ dateTime +"\r\nServer: tinyserver.colab.duke.edu\r\nContent-Type: text/html\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);
  std::cout <<"header sent \n";
  
  while((numBytes = fread(html,1,8000,html_file)) > 0){
    send(conn_state->response_socket,html,numBytes,0);
  }
  
  return 1;
}

