#include "connection.h"
#include <cstdlib>
#include <cstring>
#include <stdio.h>


/************************
 * Request class methods
 *************************/

Request::Request(char* r, char* u, char* h): request_method(r), request_URI(u), http_version(h) {
  time_t ping;
  time(&ping);
  request_bday = localtime(&ping);
  if(request_URI.length() == 0){
    request_URI = "/";
  }
  else{
    if(request_URI[0] != '/'){
      request_URI = "/" + request_URI;
    }
  }
}

void Request::addHeader(const char* key, const char* value) {
  std::string header_type(key);
  std::string header_value(value);
  
  if(!header_type.compare("If-Modified-Since")){
    headers[header_type] = header_value;}
  else{
    header_type = standardize(header_type);
    header_value = standardize(header_value);
    headers[header_type] = header_value;
  }
}

void Request::addBody(const char * body) {
  message_body = std::string(body);
}

std::string Request::standardize(std::string str){
  for(unsigned i = 0; i < str.length(); i++){
    str[i] = tolower(str[i]);
  }
  return str;
}

int Request::check_keep_alive(){
  if(headers.count("connection")){
    if(headers["connection"].find("keep-alive") == std::string::npos){
      return 0;
    }
    else{
      std::cout<<"\nCheck works\n";
      return 1;
    }
  }
  return 0;
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

/***********************
 * ConnObj class methods 
 ************************/

ConnObj::ConnObj() {
  msg_stream = NULL;
  char file1[] = "privileges/userdirs.txt";
  char file2[] = "privileges/getabledirs.txt";
  read_privileges(file1, &userdirs);
  read_privileges(file2, &getabledirs);
}

ConnObj::~ConnObj(){
  userdirs.clear();
  getabledirs.clear();
}

void ConnObj::read_privileges(char* filename, std::set<std::string>* auth){
  FILE* input_file;
  input_file = fopen(filename, "r");
  if(input_file == NULL){
    fprintf(stderr, "Error opening File");
  }
  char* nextline = NULL;
  size_t sz;
  char* ptr;

  while(getline(&nextline, &sz, input_file) > 0){
    ptr = strchr(nextline, '\n');
    if (ptr != NULL){
      *ptr = '\0';
    }
    auth->insert(std::string(nextline));
  }
  
  free(nextline);
  int check = fclose(input_file);
  if (check != 0){
    fprintf(stderr, "Error closing file!\n");
  }
}

int ConnObj::authorized(std::string type, std::string URI){
  std::string dir; 
  int loc = URI.find_last_of("/");
  if(loc == std::string::npos){
    return 0;
  }
  else{
    dir = "www" + URI.substr(0, loc + 1);
  }
  if(type == "GET" || type == "HEAD"){
    return getabledirs.count(dir);
  }
  else if(type == "PUT" || type == "POST" || type == "DELETE"){
    return userdirs.count(dir);
  }
  else{ 
    return 0;
  }
}
