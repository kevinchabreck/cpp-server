#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string>
#include "ConnObj.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include "putResponse.h"

void createdResponse(ConnObj* conn_state){ 
  std::string header;
  header+= "HTTP/1.1 201 Created\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);
    
}

void nocontentResponse(ConnObj* conn_state){
  std::string header;
  header+= "HTTP/1.1 204 No Content\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);
  
}

void putResponse(Request* req, ConnObj* conn_state){
  
  std::string requested_obj = req->request_URI;
  std::string filename;
  std::string dir;
  std:: size_t loc;
 
  int allowed = conn_state->authorized(req->request_method, req->request_URI);
  
  requested_obj.erase(0,1); 
  
  if(!allowed){  
    std::cout<<"Folder not authorized\n";
    //Call function for invalid directory response
  }

  else{
    struct stat st;
    int exists = stat(requested_obj.c_str(), &st);

    std::cout<<"Folder is authorized\n";
    FILE* dest = fopen(requested_obj.c_str(), "w");
    if (dest == NULL){
      fprintf(stderr, "Error opening file\n");
    }
        
    fprintf(dest,"%s","PLACEHOLDER"); //Replace this with writing msg body
    
    fclose(dest);
    
  
    if(exists != 0){
      std::cout<<"Created!\n";
      createdResponse(conn_state);
    }
    else{
      std::cout<<"Modified!\n";
      nocontentResponse(conn_state);
    }

}


  
  
}

