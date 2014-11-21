#include <iostream>
#include <cstring>
#include "ConnObj.h"

Request::Request(char* r, char* u, char* h): request_method(r), request_URI(u), http_version(h) {
  time_t ping;
  time(&ping);
  request_bday = localtime(&ping);
}

void Request::addHeader(const char* key, const char* value) {
  std::string header_type(key);
  std::string header_value(value);
  headers[header_type] = header_value;
}

void Request::addBody(const char * body) {
  message_body = std::string(body);
}

void Request::printRequest() {
  std::cout<< "\nRequest Type: "<< request_method << "\n"; 
  std::cout<<"Request URI: "<< request_URI <<"\n";
  std::cout<<"HTTP Version: "<< http_version <<"\n";
  std::cout << "\nOptional Headers:\n";
  for(std::map<std::string, std::string>:: iterator it = headers.begin(); it != headers.end(); ++it){
    std::cout << it->first << ": " << it->second << "\n";
  }
  std::cout<< "\n";
}

ConnObj::ConnObj() {
  msg_stream = NULL;
}