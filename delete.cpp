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
      send200(conn_state);
    }
    else {// If it was not deleted
      send404(conn_state);
    }
  }
}

