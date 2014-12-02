#include "post.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <string.h>
#include "common.h"

void postResponse(Request* req, ConnObj* conn_state){
  
  std::string requested_obj = "www/cgi-bin" + req->request_URI;
  std::string filename;
  std::string dir;
  std::size_t loc;
  std::string dateTime = getTimestamp();
  
  // parse script name out of request
  unsigned found = requested_obj.find_last_of("/");
  std::string script = requested_obj.substr(found+1);
  std::cout<<"script: "<<script<<std::endl;
  
  //Check if script exists
  struct stat st;
  int exists = stat(script.c_str(), &st);
  
  // if no script specified, or script does not exist, return empty response 
  if(script == "" || !exists){  
    std::cout<<"invalid resource: "<<req->request_URI<< std::endl;
    send204(conn_state);
    return;
  }

  // get extension of requested script
  found = script.find_last_of(".");
  std::string ext = script.substr(found+1);

  if(ext != "pl" && ext != "php"){  
    std::cout<<"invalid script type ("<<script<<")\n";
    std::cout<<"cpp-server only supports php and pearl scripts\n";
    // TODO: send a better error code here?
    send204(conn_state);
    return;
  }

  // check if request had a continue
  if((req->headers).count("expect") == 1){
    if((req->headers)["expect"].find("100-continue") != std::string::npos){
      send100(conn_state);
    }
    return;
  }

  // TODO: add args to command
  std::string cmd = ext + " cgi-bin/" + script;

  FILE* pipe = popen(cmd.c_str(), "r");
  if (!pipe){
    send500(conn_state);
  }
  char buffer[128];
  std::string result = "";
  while(!feof(pipe)) {
    if(fgets(buffer, 128, pipe) != NULL){
      result += buffer;
    }
  }
  pclose(pipe);

  // TODO: check if this is the correct status code when sending a POST response
  std::string header = "HTTP/1.1 202 ACCEPT\r\nDate: "+ dateTime +"\r\nServer:\
  tinyserver.colab.duke.edu\r\nContent-Type: text/html\r\n\r\n";
  send(conn_state->response_socket,header.c_str(), header.length(), 0);
  send(conn_state->response_socket,result.c_str(), result.length(), 0);
}

