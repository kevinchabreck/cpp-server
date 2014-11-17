#include <cstdlib>
#include <iostream>
#include <cstring>
#include <map>


class Request{

 public:
  std::string request_method;
  std::string request_URI;
  std::string http_version;
  std::map<std::string, std::string> headers;
  
  Request(char* r, char* u, char* h): request_method(r), request_URI(u), http_version(h){ }
  
  

};



class ConnObj{
  
 public:
  FILE* msg_stream;
  int response_socket;
  
  ConnObj(){
    msg_stream = NULL;
  }
 

};
