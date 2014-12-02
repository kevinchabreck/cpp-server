#include "get.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include "common.h"

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


void sendtoclient(Request* req, ConnObj* conn_state, const char* file){
  //Head and HTML buffers
  std::string header = "";
  char html[8000];
  int numBytes = 0;

  //Time Struct
  time_t ping;
  struct tm* currentTime;
  char timeBuffer[80];
  
  time(&ping);
  currentTime = localtime(&ping);
  strftime(timeBuffer,80,"%a, %d %h %G %T %z",currentTime);
  std::string dateTime = (timeBuffer);

   FILE* fileName = fopen(file,"r");
   if(fileName  == NULL){//BAD REQUEST
     send404(conn_state);
   }
   else{
      
     header+= "HTTP/1.1 202 ACCEPT\r\n";
     header+= "Date: "+ dateTime +"\r\n";
     header+= "Server: tinyserver.colab.duke.edu\r\n";
     header+= "Content-Type: text/html\r\n\r\n";
      
     send(conn_state->response_socket,header.c_str(),header.length(),0);
      
     while((numBytes = fread(html,1,8000,fileName)) > 0){
       send(conn_state->response_socket,html,numBytes,0);
     }
   }
   fclose(fileName); 
}




int isDefault(Request* req,ConnObj* conn_state){
  if(req->request_URI == "/." || req->request_URI == "/"){
    struct stat st;
    int index_html = stat("www/index.html", &st);
    int index_htm = stat("www/index.htm", &st);
    int default_htm = stat("www/default.htm", &st);
    int bad = 0;
    FILE* defaultFile;

    if(!index_html){
      sendtoclient(req,conn_state,"www/index.html");
    }
    else if(!index_htm){
      sendtoclient(req,conn_state,"www/index.htm");
    }
    else if(!default_htm){
      sendtoclient(req,conn_state,"www/default.htm");
    }
    else{
      send404(conn_state);
      return -1;
    }
    return 1;
  }
  return 0;
}



void getResponse(Request* req, ConnObj* conn_state){
  if(isDefault(req,conn_state) == 0){
    std::string file = "www/" + req->request_URI;
    sendtoclient(req,conn_state, file.c_str());
  }
  else {
    //DO NOTHING, 'isDefault' function already sent file
  }
}
