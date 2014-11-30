#ifndef __CONNOBJ_H__
#define __CONNOBJ_H__

#include <iostream>
#include <map>
#include <set>

class Request{

 public:
  std::string request_method;
  std::string request_URI;
  std::string http_version;
  std::map<std::string, std::string> headers;
  std::string message_body;
  struct tm* request_bday;

  Request(char* r, char* u, char* h);  
  void addHeader(const char* key, const char* value);
  void addBody(const char * body);
  std::string standardize(std::string str);
  int check_keep_alive();
  void printRequest();
};


class ConnObj{
 public:
  std::set <std::string> userdirs;
  std::set <std::string> getabledirs;
  FILE* msg_stream;
  int response_socket;
  int keep_alive;

  ConnObj();
  int authorized(std::string type, std::string dir);
  
 private:
  void read_privileges(char* filename, std::set<std::string>& auth);
};

#endif
