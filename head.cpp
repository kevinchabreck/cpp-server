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

bool beenModified(Request* req){
  if(req->headers.count("If-Modified-Since")){
    struct tm reqTime;
    std::string requestTime = (req->headers["If-Modified-Since"]);
    if(strptime(requestTime.c_str(),"%a, %d %b %Y %H:%M:%S -0500", &reqTime) == NULL){
      return true; //could not convert time
    }
    //Get last modified info from file
    struct stat fileStat;
    stat(req->request_URI.c_str(),&fileStat);
    int time = difftime(fileStat.st_mtime, mktime(&reqTime));
    if(time <= 0){
      return false;
    }
    else {
      return true;
    }
  }
  return true;
}

std::string get_relpath(std::string URI){
  char* absolute_path = NULL;
  std::string path = "www" + URI;
  absolute_path = realpath(path.c_str(), absolute_path);
  if(absolute_path == NULL){
    return "NULL";
  }
  std::string requestObject(absolute_path);
  free(absolute_path);
  std::string rel_path = requestObject.substr(requestObject.rfind("www"), std::string::npos);
  struct stat st;
  stat(rel_path.c_str(), &st);
  if(S_ISDIR(st.st_mode)){
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
    send403(conn_state);    
    return 0;
  }
  else{
    if(!beenModified(req)){
      std::string header;
      std::string dateTime = getTimestamp();
      header+= "HTTP/1.1 202 ACCEPT\r\n";
      header+= "Date: "+ dateTime +"\r\n";
      header+= "Server: tinyserver.colab.duke.edu\r\n";
      if(allowsCompression(req)){  // COMPRESSION IS REQUESTED
        header+= "Content-Encoding: gzip\r\n";
      }
      header+= "Content-Type: " + getContentType(rel_path)+ "\r\n\r\n";
      send(conn_state->response_socket,header.c_str(),header.length(),0);  
      return 1;
    } 
    send304(conn_state);
    return 1; 
  }
}
