//========================================
//Programmers: Sean Murray, Kevin Chabreck, Julien Mansier
//Date: 11-25-2014
//File: response.cpp
//
//Description:
//This program is called run when a client requests a 'Get'
//The logic will record the current timestamp for an accurate header
//It will then check if the file can be opened
//If it cannot be opened then it sends a 404 Bad Request to the client with NO CONTENT
//If it does open, it response with a 202 ACCEPT and sends the content
//
//Future Functionality: 
//  1) Commpression using zlib
//  2) If-Modified 
//=========================================

#include "get.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <zlib.h>
#include "badRequest.h"

bool beenModified(struct tm* ping, std::string file){
  struct tm* clock;
  struct stat attrib;
  stat(file.c_str(), &attrib);
  clock = gmtime(&(attrib.st_mtime));

  //std::cout << (long long) clock;
  //std::cout << " ";
  //std::cout << (long long) ping;
  //std::cout << "\n";

  //  std::cout << boost::filesystem::last_write_time(file);

  if( (long long)clock - (long long)ping > 0)
    return false;
  
  return true;
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
  std::string file =  req->request_URI;
 
  /*
  if(!beenModified(currentTime, file)){
  header+="HTTP/1.1 304 Not Modified\r\nDate: "+ dateTime +"\r\nServer: tinyserver.colab.duke.edu\r\nContent-Type: text/html\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);
  std::cout << "\n304 Not Modified";
  return 1;
  } */

 
  //File
  FILE* html_file = fopen(file.c_str(), "r");
  if(html_file  == NULL){
    //BAD REQUEST
    badRequest(conn_state);
    return 0;
  }



  header+= "HTTP/1.1 202 ACCEPT\r\nDate: "+ dateTime +"\r\nServer: tinyserver.colab.duke.edu\r\nContent-Type: text/html\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);
  std::cout <<"header sent \n";
  
  while((numBytes = fread(html,1,8000,html_file)) > 0){
    send(conn_state->response_socket,html,numBytes,0);
  }
  
  return 1;
}
