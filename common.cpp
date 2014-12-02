#include "common.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include "connection.h"

void send100(ConnObj* conn_state){
  std::string header;
  header+="HTTP/1.1 100 Continue\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);
}

void send201(ConnObj* conn_state){//Msg to be called when new resource is created on behalf of user 
  std::string header;
  header+= "HTTP/1.1 201 Created\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);   
}

// status code for a "no content" response
void send204(ConnObj* conn_state){
  std::string header;
  header+= "HTTP/1.1 204 No Content\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0); 
}

void send400(ConnObj* conn_state){
  std::string header;

  //Time Struct
  time_t ping;
  struct tm* currentTime;
  char timeBuffer[80];
  
  time(&ping);
  currentTime = localtime(&ping);
  strftime(timeBuffer,80,"%a, %d %h %G %T %z",currentTime);
 
  std::string dateTime = (timeBuffer);

  header+="HTTP/1.1 400 Bad Request\r\nDate: "+ dateTime +"\r\nServer: tinyserver.colab.duke.edu\r\nContent-Type: text/html\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);
  sendHTML(conn_state, "400");
  
}

void send401(ConnObj* conn_state){
  std::string header;

  //Time Struct
  time_t ping;
  struct tm* currentTime;
  char timeBuffer[80];
  
  time(&ping);
  currentTime = localtime(&ping);
  strftime(timeBuffer,80,"%a, %d %h %G %T %z",currentTime);
 
  std::string dateTime = (timeBuffer);

  header+="HTTP/1.1 401 UNAUTHORIZED\r\nDate: "+ dateTime +"\r\nServer: tinyserver.colab.duke.edu\r\nContent-Type: text/html\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);
  sendHTML(conn_state, "401");
  
}



void send403(ConnObj* conn_state){
  std::string header;
 
  time_t ping;
  struct tm* currentTime;
  char timeBuffer[80];
  
  time(&ping);
  currentTime = localtime(&ping);
  strftime(timeBuffer,80,"%a, %d %h %G %T %z",currentTime);
 
  std::string dateTime = (timeBuffer);

  header+="HTTP/1.1 403 Forbidden\r\nDate: "+ dateTime +"\r\nServer: tinyserver.colab.duke.edu\r\nContent-Type: text/html\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);
  sendHTML(conn_state, "403");


}   

void send404(ConnObj* conn_state){
  //Head and HTML buffers
  std::string header;

  //Time Struct
  time_t ping;
  struct tm* currentTime;
  char timeBuffer[80];
  
  time(&ping);
  currentTime = localtime(&ping);
  strftime(timeBuffer,80,"%a, %d %h %G %T %z",currentTime);
  std::string dateTime = (timeBuffer);

  header+="HTTP/1.1 404 Bad Request\r\nDate: "+ dateTime +"\r\nServer: tinyserver.colab.duke.edu\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);

  sendHTML(conn_state, "404");
  
}



void send411(ConnObj* conn_state){
  std::string header;
 
  time_t ping;
  struct tm* currentTime;
  char timeBuffer[80];
  
  time(&ping);
  currentTime = localtime(&ping);
  strftime(timeBuffer,80,"%a, %d %h %G %T %z",currentTime);
 
  std::string dateTime = (timeBuffer);

  header+="HTTP/1.1 411 Length Required\r\nDate: "+ dateTime +"\r\nServer: tinyserver.colab.duke.edu\r\nContent-Type: text/html\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);
  sendHTML(conn_state, "411");

}   


// status code for an internal server error
void send500(ConnObj* conn_state){
  std::string header;
  header+= "HTTP/1.1 500 Internal Server Error\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0); 
}

void sendHTML(ConnObj* conn_state, std::string status){

  char html[8000];
  int numBytes = 0;  
  std::string filename = "www/error_files/" + status + ".html";
  FILE* html_file = fopen(filename.c_str(), "r");
  if(html_file != NULL){
    while((numBytes = fread(html,1,8000,html_file)) > 0){
      send(conn_state->response_socket,html,numBytes,0);
    }
    
    int check = fclose(html_file);
    if(check != 0){
      fprintf(stderr, "Error closing file!\n");
    }
  }
  

}




std::string getTimestamp(){
  //Time Struct
  time_t ping;
  struct tm* currentTime;
  char timeBuffer[80];
  
  time(&ping);
  currentTime = localtime(&ping);
  strftime(timeBuffer,80,"%a, %d %h %G %T %z",currentTime);
  std::string dateTime = (timeBuffer);
  return dateTime;
}
