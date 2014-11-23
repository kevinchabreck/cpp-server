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
  std::string message_body;
  struct tm* request_bday;
   
  Request(char* r, char* u, char* h): request_method(r), request_URI(u), http_version(h){ 

    time_t ping;
    time(&ping);
    request_bday = localtime(&ping);
}
  
  void addHeader(const char* key, const char* value){
    std::string header_type(key);
    std::string header_value(value);
    header_type = standardize(header_type);
    header_value = standardize(header_value);

    headers[header_type] = header_value;
    
  }

  void addBody(const char * body){
    message_body = std::string(body);
  }
  
  std::string standardize(std::string str){
    for(unsigned i = 0; i < str.length(); i++){
      str[i] = tolower(str[i]);
    }

    return str;
  }
  
  void printRequest(){

    std::cout<< "\nRequest Type: "<< request_method << "\n"; 
    std::cout<<"Request URI: "<< request_URI <<"\n";
    std::cout<<"HTTP Version: "<< http_version <<"\n";
    
    std::cout << "\nOptional Headers:\n";
    for(std::map<std::string, std::string>:: iterator it = headers.begin(); it != headers.end(); ++it){
      std::cout << it->first << ": " << it->second << "\n";
    }
    std::cout<< "\n";
  }

};


class ConnObj{
  
 public:
  FILE* msg_stream;
  int response_socket;
  int keep_alive;
  ConnObj(){
    msg_stream = NULL;
  }
 

};
