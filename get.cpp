#include "get.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <zlib.h>
#include "common.h"
#include "head.h"


//Checks to see if client allows compression
bool allowsCompression(Request* req){
  if((req->headers).count("accept-encoding") == 1){
    if((req->headers)["accept-encoding"].find("gzip") != std::string::npos){
      return true;
    }
  }
  return false;
}

void  sendBody(Request* req, ConnObj* conn_state){
  //Head and HTML buffers
  char html[8000];
  int numBytes = 0;
  bool compressFailed = false;
  std::string rel_path = get_relpath(req->request_URI);
  FILE* fileName = fopen(rel_path.c_str(),"r");
  if(fileName  == NULL){ // BAD REQUEST
    send404(conn_state);
  }
  else{
    if(allowsCompression(req)){
      gzFile output = gzopen("compressed_file","wb");
      if(!output){
        std::cout << "Could not open/creat compressed file\n";
        send500(conn_state);
        compressFailed = true;
      }
      else{
        std::cout << "Compressing file\n";
        while((numBytes = fread(html,1,sizeof(html),fileName)) > 0){
          gzwrite(output, html,numBytes);
        }
        gzclose(output);
        fclose(fileName);
        FILE* fileName = fopen("compressed_file","r");
        if(!fileName){
          std::cout << "Could not open compressed file!\n";
          compressFailed = true;
          send500(conn_state);
        }
        else{
          compressFailed = false;
        }
      }
    }
    if(compressFailed == false){
      while((numBytes = fread(html,1,8000,fileName)) > 0){
        send(conn_state->response_socket,html,numBytes,0);
      }
      fclose(fileName);
      if(remove("compressed_file") != 0){
        std::cout << "Compressed file could not be deleted!\n";
      }
    }
    else{
      std::cout << "Could not send to client!\n";
      send500(conn_state);
    }
  }
}

void getResponse(Request* req, ConnObj* conn_state){
  int ok = headResponse(req, conn_state);  
  if (ok){
    std::cout<<"GOING TO SEND BODY!!!\n";
    sendBody(req, conn_state);
  }
}
