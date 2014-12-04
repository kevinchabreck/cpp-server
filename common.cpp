#include "common.h"
#include <iostream>
#include <map>
#include <algorithm>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include "connection.h"

extern int mode;

void send100(ConnObj* conn_state){
  std::string header;
  header += "HTTP/1.1 100 Continue\r\n";
  header += "Date: "+getTimestamp()+"\r\n";
  header += "Server: tinyserver.colab.duke.edu\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);
  log("Continue Response Sent");
}

void send200(ConnObj* conn_state){
  std::string header;
  header += "HTTP/1.1 200 OK\r\n";
  header += "Date: "+getTimestamp()+"\r\n";
  header += "Server: tinyserver.colab.duke.edu\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);   
}

//Msg to be called when new resource is created on behalf of user
void send201(ConnObj* conn_state){
  std::string header;
  header += "HTTP/1.1 201 Created\r\n";
  header += "Date: "+getTimestamp()+"\r\n";
  header += "Server: tinyserver.colab.duke.edu\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);   
  log("Put Response Fulfilled, Object Created");
}

// status code for a "no content" response
void send204(ConnObj* conn_state){
  std::string header;
  header += "HTTP/1.1 204 No Content\r\n";
  header += "Date: "+getTimestamp()+"\r\n";
  header += "Server: tinyserver.colab.duke.edu\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);
  if(mode==DEBUG){log("204 (No Content) Sent to Client");}
}

void send304(ConnObj* conn_state){
  std::string header;
  header += "HTTP/1.1 304 Not Modified\r\n";
  header += "Date: "+getTimestamp()+"\r\n";
  header += "Server: tinyserver.colab.duke.edu\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);
  if(mode==DEBUG){log("304 (Not Modified) Sent to Client");}
}

void send400(ConnObj* conn_state){
  std::string header;
  header += "HTTP/1.1 400 Bad Request\r\n";
  header += "Date: "+getTimestamp()+"\r\n";
  header += "Server: tinyserver.colab.duke.edu\r\n";
  header += "Content-Type: text/html\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);
  sendHTML(conn_state, "400");
}

void send401(ConnObj* conn_state){
  std::string header;
  header += "HTTP/1.1 401 UNAUTHORIZED\r\n";
  header += "Date: "+getTimestamp()+"\r\n";
  header += "Server: tinyserver.colab.duke.edu\r\n";
  header += "Content-Type: text/html\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);
  sendHTML(conn_state, "401");
  log("Request not fulfilled, client not authorized");
}

void send403(ConnObj* conn_state){
  std::string header;
  header += "HTTP/1.1 403 Forbidden\r\n";
  header += "Date: "+getTimestamp()+"\r\n";
  header += "Server: tinyserver.colab.duke.edu\r\n";
  header += "Content-Type: text/html\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);
  sendHTML(conn_state, "403");
}   

void send404(ConnObj* conn_state){
  std::string header;
  header += "HTTP/1.1 404 Not Found\r\n";
  header += "Date: "+getTimestamp()+"\r\n";
  header += "Server: tinyserver.colab.duke.edu\r\n";
  header += "Content-Type: text/html\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);
  sendHTML(conn_state, "404");
  log("Request not fulfilled, bad URI, sent 404");
}

void send405(ConnObj* conn_state){
  std::string header;
  header += "HTTP/1.1 405 Method Not Allowed\r\n";
  header += "Date: "+getTimestamp()+"\r\n";
  header += "Server: tinyserver.colab.duke.edu\r\n";
  header += "Allow: .php, .pl\r\n";
  header += "Content-Type: text/html\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);
  sendHTML(conn_state, "405");
  log("Request not fulfilled, method not allowed, sent 405");
}

void send411(ConnObj* conn_state){
  std::string header;
  header += "HTTP/1.1 411 Length Required\r\n";
  header += "Date: "+getTimestamp()+"\r\n";
  header += "Server: tinyserver.colab.duke.edu\r\n";
  header += "Content-Type: text/html\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);
  sendHTML(conn_state, "411");
  if(mode==DEBUG){log("Request not fulfilled, client did not send length, sent 411");}
}   

// status code for an internal server error
void send500(ConnObj* conn_state){
  std::string header;
  header+= "HTTP/1.1 500 Internal Server Error\r\n";
  header += "Date: "+getTimestamp()+"\r\n";
  header += "Server: tinyserver.colab.duke.edu\r\n";
  header += "Content-Type: text/html\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);
  sendHTML(conn_state, "500");
  log("Request not fulfilled, encountered internal error, sent 500");
}

void send501(ConnObj* conn_state){
  std::string header;
  header+= "HTTP/1.1 501 Not Implemented Error\r\n";
  header += "Date: "+getTimestamp()+"\r\n";
  header += "Server: tinyserver.colab.duke.edu\r\n";
  header += "Content-Type: text/html\r\n\r\n";
  send(conn_state->response_socket,header.c_str(),header.length(),0);
  sendHTML(conn_state, "501");
 }

/****************
* Helper methods
*****************/

void log(std::string message) {
  if(mode == STANDARD){
    std::ofstream out;
    out.open("logs/server.log", std::ios::app);
    out<<"<"<<getTimestamp()<<"> "<<message<<"\n";
  }
  else if(mode == DEBUG){
    std::cout<<message<<"\n";
  }
}

// sends the specified error file
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
      if(mode==DEBUG){log("Error Closing File");};
    }
  }
}

// returns a string of the current time
std::string getTimestamp(){
  time_t ping;
  struct tm* currentTime;
  char timeBuffer[80];
  time(&ping);
  currentTime = localtime(&ping);
  strftime(timeBuffer,80,"%a, %d %h %G %T %z",currentTime);
  std::string dateTime = (timeBuffer);
  return dateTime;
}

std::string getContentType(std::string filePath){
  std::map<std::string, std::string> content;
  // build map of a few common filetypes
  content["jpeg"] = "image";
  content["png"] = "image";
  content["html"] = "text";
  content["txt"] = "text";
  content["rtf"] = "text";
  // parse file name out of path
  unsigned found = filePath.find_last_of("/");
  std::string fileName = filePath.substr(found+1);
  // get file extension
  found = fileName.find_last_of(".");
  std::string ext = fileName.substr(found+1);
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
  return content[ext] + "/" + ext;
}
