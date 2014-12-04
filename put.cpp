#include "put.h"
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
#include "head.h"

extern int mode;

void putResponse(Request* req, ConnObj* conn_state){
  std::string requested_obj = "www" + req->request_URI;
  std::string filename;
  std::string dir;
  //Check if folder is authorized for PUTs
  std::string path = req->request_URI;
  int loc = path.find_last_of("/");
  std::string rel_path = get_relpath(path.substr(0, loc + 1));
  int allowed = conn_state->authorized(req->request_method, rel_path.substr(3, std::string::npos));
  if(!allowed){  
    send403(conn_state);
  }
  else{
    //If function needs a continue response, send it
    if((req->headers).count("expect") == 1){
      if((req->headers)["expect"].find("100-continue") != std::string::npos){
        send100(conn_state);
      }
    }
    //Check if file exists already
    struct stat st;
    int exists = stat(requested_obj.c_str(), &st);
    if(exists == 0){
      if(S_ISDIR(st.st_mode)){
        if(mode==DEBUG){log("Tried to override a folder with file of the same name");}
        send403(conn_state);
        return;
      }
    }
    //Open file to write into
    FILE* dest = fopen(requested_obj.c_str(), "w");
    if (dest == NULL){
      if(mode==DEBUG){log("Opening File Failed");}
      send500(conn_state);
      return;
    }
    //Figue out how long body will be
    long int n = 0;
    char buffer[8000];
    if((req->headers).count("content-length") == 0){
      send411(conn_state);
      return;
    }
    std::string size = req->headers["content-length"];
    std::cout << "Content length expected: "<< size << "\n";
    char* ptr = NULL;
    long int total = strtol(size.c_str(), &ptr, 10);
    long int sum = 0;
    int to_read;
    //Read 8000 bytes at a time from body, then read remaining
    //Keep running sum so can read exactly whats there and not end
    //up blocking
    while (sum < total){
      if(total - sum > 8000){
        to_read = 8000;
      }
      else{
        to_read = total - sum;
      }
      n = fread(buffer, 1, to_read, conn_state->msg_stream); 
      fwrite(buffer,1, to_read, dest);
      bzero(buffer, 8000);
      sum = sum + n;
    }   
    fclose(dest);
    char mode[]= "0755";
    int p;
    p = strtol(mode, 0, 8);
    chmod(requested_obj.c_str(), p);
    //Call appropriate response msg
    if(exists != 0){
      send201(conn_state);
    }
    else{
      send204(conn_state);
    }
  }
}
