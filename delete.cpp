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
  std::string requestObject = req->request_URI;
  requestObject.erase(0,1);
  // Check to see if the directory has user permissions
  int allowed = conn_state->authorized(req->request_method, req->request_URI);
  if(!allowed){
    send401(conn_state);
  }
  else{
    if(remove(requestObject.c_str()) == 0){
      // File was deleted
      send200(conn_state);
    }
    else {
      // If it was not deleted
      send404(conn_state);
    }
  }
}
