#include <iostream>
#include "options.h"
#include "ConnObj.h"
#include <time.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>


void optionsResponse(Request* req, ConnObj* conn_state){
  std::string header;

  //Time Struct
  time_t ping;
  struct tm* currentTime;
  char timeBuffer[80];
  
  time(&ping);
  currentTime = localtime(&ping);
  strftime(timeBuffer,80,"%a, %d %h %G %T %z",currentTime);
  std::string dateTime = (timeBuffer);


  header+= "HTTP/1.1 200 OK\r\n";
  header+= "Date: " + dateTime + "\r\n";
  header+= "Server: tinyserver.colab.duke\r\n";
  header+= "Allow: HEAD, GET, PUT, POST, DELETE, OPTIONS\r\n";
  header+= "Content-Type: http/images/audio/text/video\r\n";

  send(conn_state->response_socket,header.c_str(),header.length(),0);
  std::cout << header;
}
