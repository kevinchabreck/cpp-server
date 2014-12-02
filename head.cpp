#include "head.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "connection.h"
#include "common.h"
#include "get.h"


void headResponse(Request* req, ConnObj* conn_state){
  //Head and HTML buffers
  std::string header;
  

  //Time Struct
  time_t ping;
  struct tm* currentTime;
  char timeBuffer[80];
  
  time(&ping);
  currentTime = localtime(&ping);
  strftime(timeBuffer,80,"%a, %d %h %G %T %z",currentTime);
 
  std::string dateTime = (timeBuffer);



  struct stat st;
  std::string requestObject = "www" + req->request_URI;
  int fileStatus = stat(requestObject.c_str(),&st);
 

  // DOES THE FILE EXIST?
  if(fileStatus == 0){
    if(allowsCompression(req)){  // COMPRESSION IS REQUESTED
	header+= "HTTP/1.1 202 ACCEPT\r\n";
	header+= "Date: "+ dateTime +"\r\n";
	header+= "Server: tinyserver.colab.duke.edu\r\n";
	header+= "Content-Type: text/html\r\n";
	header+= "Content-Encoding: gzip\r\n\r\n";
	send(conn_state->response_socket,header.c_str(),header.length(),0);
      }
      else {	      // COMPRESSION IS NOT REQUESTED
	header+= "HTTP/1.1 202 ACCEPT\r\n";
	header+= "Date: "+ dateTime +"\r\n";
	header+= "Server: tinyserver.colab.duke.edu\r\n";
	header+= "Content-Type: text/html\r\n";
	send(conn_state->response_socket,header.c_str(),header.length(),0);
      }
      }
    else{ // REQUESTED URI DOES NOT EXIST
      send400(conn_state);
    }
  }
