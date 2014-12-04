#include "post.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <algorithm>
#include <sys/socket.h>
#include <string.h>
#include "common.h"

void postResponse(Request* req, ConnObj* conn_state){
  
  std::string requested_obj =  req->request_URI;
  std::string filename;
  std::string dir;
  std::size_t loc;
  std::string dateTime = getTimestamp();
  
  // parse script name out of request
  unsigned found = requested_obj.find_last_of("/");
  std::string script = requested_obj.substr(found+1);
    
  //Check if script exists
  struct stat st;
  int not_exists = stat(("cgi-bin/"+script).c_str(), &st);
  
  // if no script specified, or script does not exist, return empty response 
  if(script == "" || not_exists == -1 ){  
    send404(conn_state);
    return;
  }

  // get extension of requested script
  found = script.find_last_of(".");
  std::string ext = script.substr(found+1);

  if(ext != "pl" && ext != "php"){  
    if(mode == DEBUG){log("Invalid Script Type Requested");}
    send405(conn_state);
    return;
  }

  // check if request had a continue
  if((req->headers).count("expect") == 1){
    if((req->headers)["expect"].find("100-continue") != std::string::npos){
      send100(conn_state);
    }
    return;
  }

 
  std::string cmd = ext + " cgi-bin/" + script;

  // parse arguments out of body (if there are any)
  if((req->headers).count("content-length")){
    long int n = 0;
    char buffer[8000];
    std::string size = req->headers["content-length"];
    char* ptr = NULL;
    long int total = strtol(size.c_str(), &ptr, 10);
    long int sum = 0;
    int to_read;

    std::string args = "";

    while (sum < total){
      if(total - sum > 8000){
        to_read = 8000;
      }
      else{
        to_read = total - sum;
      }
      n = fread(buffer, 1, to_read, conn_state->msg_stream);
      args += buffer;
      bzero(buffer, 8000);
      sum = sum + n;
    }
    
    std::replace(args.begin(), args.end(), '&', ' ');
    cmd += " " + args;
  }

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

  send200(conn_state);
  send(conn_state->response_socket,header.c_str(), header.length(), 0);
  send(conn_state->response_socket,result.c_str(), result.length(), 0);
}

