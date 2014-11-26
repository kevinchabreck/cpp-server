//========================================
//Programmers: Sean Murray, Kevin Chabreck, Julien Mansier
//Date: 11-25-2014
//
//Description:
//This program is called run when a client requests a 'Delete'
//We do not want to support this functionality for security purposes
//The the appropiate request method response is handled by server.cpp
//=========================================

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ConnObj.h"
#include "delete.h"
#include <sys/types.h>
#include <sys/socket.h>


void deleteResponse(Request* req, ConnObj* conn_state){
  std::string header;

  //Time Struct
  time_t ping;
  struct tm* currentTime;
  char timeBuffer[80];
  
  time(&ping);
  currentTime = localtime(&ping);
  strftime(timeBuffer,80,"%a, %d %h %G %T %z",currentTime);
  std::string dateTime = (timeBuffer);

  req->request_URI.erase(0,1);
  std::string file = "www/" + req->request_URI;


  header+="HTTP/" + req->http_version + "403 FORBIDDEN\r\nDate: "+ dateTime +"\r\nServer: tinyserver.colab.duke.edu\r\nContent-Type: text/html\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);

}

