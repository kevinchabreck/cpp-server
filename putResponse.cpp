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
#include <string.h>


void createdResponse(ConnObj* conn_state){//Msg to be called when new resource is created on behalf of user 
  std::string header;
  header+= "HTTP/1.1 201 Created\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);
    
}

void nocontentResponse(ConnObj* conn_state){//Msg to be called when resource is modified on behalf of user
  std::string header;
  header+= "HTTP/1.1 204 No Content\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);
  
}

void cont_response(ConnObj* conn_state){ //Msg to acknowledge client can send rest of msg body
  std::string header;
  header+="HTTP/1.1 100 Continue\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);
  
}

void putResponse(Request* req, ConnObj* conn_state){
  
  std::string requested_obj = req->request_URI;
  std::string filename;
  std::string dir;
  std:: size_t loc;
  

  //Check if folder is authorized for PUTs
  int allowed = conn_state->authorized(req->request_method, req->request_URI);
  
  //ERASE opening slash
  requested_obj.erase(0,1); 
  
  
  if(!allowed){  
    std::cout<<"Folder not authorized\n";
    //Call function for invalid directory response
  }

  //If function needs a continue response, send it
  else{
    if((req->headers).count("expect") == 1){
      if((req->headers)["expect"].find("100-continue") != std::string::npos){
	cont_response(conn_state);
      }
    }
    
    //Check if file exists already
    struct stat st;
    int exists = stat(requested_obj.c_str(), &st);

    
    std::cout<<"Folder is authorized\n";
    
    //Open file to write into
    FILE* dest = fopen(requested_obj.c_str(), "w");
    if (dest == NULL){
      fprintf(stderr, "Error opening file\n");
    }
   
    //Figue out how long body will be
    long int n = 0;
    char buffer[8000];
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
      if(total - sum > 8000){to_read = 8000;}
      else{to_read = total - sum;}
      n = fread(buffer, 1, to_read, conn_state->msg_stream); 
      fwrite(buffer,1, to_read, dest);
      bzero(buffer, 8000);
      sum = sum + n;
      std::cout <<" Received so far: "<<sum << "\n";
    }   
    
    fclose(dest);
    //Call appropriate response msg
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

