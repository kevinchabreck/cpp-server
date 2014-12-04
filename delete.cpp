//========================================
//Programmers: Sean Murray, Kevin Chabreck, Julien Mansier
//Date: 11-25-2014
//
//Description:
//This program will first check that the request directory has user permissions
//If so, it will try to delete the requested file
//The correct responses are: Unathorized, OK, and Not Found
//=========================================

#include "delete.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "connection.h"
#include "common.h"

void deleteResponse(Request* req, ConnObj* conn_state){
  std::string header;
  std::string requestObject = req->request_URI;
  //Time Struct
  time_t ping;
  struct tm* currentTime;
  char timeBuffer[80];
  
  //Current Time for header
  time(&ping);
  currentTime = localtime(&ping);
  strftime(timeBuffer,80,"%a, %d %h %G %T %z",currentTime);
  std::string dateTime = (timeBuffer);

  // Check to see if the directory has user permissions
  requestObject.erase(0,1);
  int allowed = conn_state->authorized(req->request_method, req->request_URI);
  
  
  if(!allowed){
    // Directory does not have user permission
    send401(conn_state);
   }

  else{ // Directory has user permissions
    //Try to remove the file
    if(remove(requestObject.c_str()) == 0){ // IF == 0, File was deleted
      header+="HTTP/" + req->http_version + "200 OK\r\nDate: "+ dateTime +"\r\nServer: tinyserver.colab.duke.edu\r\nContent-Type: text/html\r\n\r\n";
      send(conn_state->response_socket,header.c_str(),header.length(),0);
      log("Delete Successful");
    }
    else {// If it was not deleted
      send404(conn_state);
      
    }
  }
}

