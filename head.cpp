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


//What to do if request is for a directory rather than a file?
//Are we actually doing compression now?

std::string get_relpath(std::string URI){
  char* absolute_path = NULL;
  std::string path = "www" + URI;
  absolute_path = realpath(path.c_str(), absolute_path);
  
  if(absolute_path == NULL){
    std::cout<<"Absolute path NULL! File not Found\n";
    return "NULL";
  }
  // std::cout<< absolute_path << "\n";
  std::string requestObject(absolute_path);
  free(absolute_path);
  std::string rel_path = requestObject.substr(requestObject.rfind("www"), std::string::npos);
  //std::cout <<rel_path << "\n";
  if(rel_path == "www"){
    rel_path += "/index.html";
  }
  
  return rel_path;
}


int headResponse(Request* req, ConnObj* conn_state){
           
  std::string rel_path = get_relpath(req->request_URI);
    
  if(rel_path == "NULL"){
    send404(conn_state);
    return 0;
  }
  

  int allowed = conn_state->authorized(req->request_method, rel_path.substr(3, std::string::npos));
  if(!allowed){
    std::cout<< "Folder not authorized\n";
    send403(conn_state);    
    return 0;
  }

  else{
    std::string header;
    time_t ping;
    struct tm* currentTime;
    char timeBuffer[80];
    time(&ping);
    currentTime = localtime(&ping);
    strftime(timeBuffer,80,"%a, %d %h %G %T %z",currentTime);
    std::string dateTime = (timeBuffer);
    
    header+= "HTTP/1.1 202 ACCEPT\r\n";
    header+= "Date: "+ dateTime +"\r\n";
    header+= "Server: tinyserver.colab.duke.edu\r\n";
    if(allowsCompression(req)){  // COMPRESSION IS REQUESTED
      //header+= "Content-Encoding: gzip\r\n";
    }
    header+= "Content-Type: text/html\r\n\r\n";
    send(conn_state->response_socket,header.c_str(),header.length(),0);  
    return 1;
  }
  
}


