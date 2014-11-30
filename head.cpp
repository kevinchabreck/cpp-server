//========================================
//Programmers: Sean Murray, Kevin Chabreck, Julien Mansier
//Date: 11-25-2014
//File: head.cpp
//
//Description:
//First collects current timestamp
//Sends simple 200 OK response to the client 
//=========================================

#include "head.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "connection.h"

void headResponse(Request* req, ConnObj* conn_state){
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

  
  header+="HTTP/1.1 200 OK\r\nDate: "+ dateTime +"\r\nServer: tinyserver.colab.duke.edu\r\nContent-Type: text/html\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);

}
