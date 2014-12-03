#include "get.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include "common.h"
#include "head.h"

bool beenModified(struct tm* ping, std::string file){
  struct tm* clock;
  struct stat attrib;
  stat(file.c_str(), &attrib);
  clock = gmtime(&(attrib.st_mtime));

  if( (long long)clock - (long long)ping > 0){
    return false;
  }
  
  return true;
}


//Checks to see if client allows compression
bool allowsCompression(Request* req){
  if((req->headers).count("accept-encoding") == 1){
    if((req->headers)["accept-encoding"].find("gzip") != std::string::npos){
      return true;
    }
  }
  return false;
}

void sendBody(Request* req, ConnObj* conn_state){
  //Head and HTML buffers
  char html[8000];
  int numBytes = 0;
  
  std::string rel_path = get_relpath(req->request_URI);
  
  FILE* fileName = fopen(rel_path.c_str(),"r");
  if(fileName  == NULL){//BAD REQUEST
     send404(conn_state);
   }
   else{
     while((numBytes = fread(html,1,8000,fileName)) > 0){
       send(conn_state->response_socket,html,numBytes,0);
     }
     fclose(fileName); 
   }
}

void getResponse(Request* req, ConnObj* conn_state){
  
  int ok = headResponse(req, conn_state);
  
  if (ok){
    std::cout<<"GOING TO SEND BODY!!!\n";
    sendBody(req, conn_state);
  }
  
}
