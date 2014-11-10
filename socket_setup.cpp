#include <sys/types.h>
#include <sys/socket.h>
#include <cstdlib>
#include <exception>
#include <assert.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <cerrno>
#include <cstring>
#include <unistd.h>


int main(int argc, char ** argv) {
 
  struct sockaddr_in client_addr;

  socklen_t client_addr_size;
  client_addr_size = sizeof(client_addr); 

  int server_socket;
  int response_socket;
  int status;
  

  struct addrinfo host_info;
  struct addrinfo* host_info_list;
 

  memset(&host_info, 0 , sizeof(host_info));
 

  host_info.ai_family = AF_INET;
  host_info.ai_socktype = SOCK_STREAM;


  host_info.ai_flags = AI_PASSIVE;
  status = getaddrinfo(NULL, "8080", &host_info, &host_info_list);
  if(status < 0){
    fprintf(stderr, "error getting host info");
  }
   

 
  server_socket = socket (AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0){
    fprintf(stderr, "COULD NOT OPEN SOCKET");
  }

  
  
 

  int yes = 1;
  setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

  if (bind(server_socket, host_info_list->ai_addr, host_info_list->ai_addrlen) < 0){
    fprintf(stderr,"BINDING FAILED: %d", errno);
    std::cout<<std::strerror(errno)<<"\n";
  }

  if(listen(server_socket, 5)< 0){
    fprintf(stderr, "Listening Failed");
  }



 
  std::cout<<"waiting for connection\n";
  response_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
  std::cout<<"Connection received\n";
  if(response_socket < 0){
    fprintf(stderr, "ERROR ACCEPTING");
  }

  
  FILE* msg_stream = fdopen(response_socket, "r");

  if (msg_stream == NULL){
    fprintf(stderr, "Error: %s\n",strerror(errno)); 

  }

  size_t sz;
  char* request_method =NULL;
  char* request_URI = NULL;
  char* http_version = NULL;
  char * ptr = NULL;

  getdelim(&request_method, &sz, ' ', msg_stream);
  ptr = strchr(request_method, ' ');
  *ptr = '\0';

  getdelim(&request_URI, &sz, ' ', msg_stream);
  ptr = strchr(request_URI, ' ');
  *ptr = '\0';
   
  getdelim(&http_version, &sz, '\n', msg_stream);
  ptr = strchr(http_version, '\n');
  *ptr = '\0';

  std::cout<< "Request Type: "<< request_method << "\n"; 
  std::cout<<"Request URI: "<< request_URI <<"\n";
  std::cout<<"HTTP Version: "<< http_version <<"\n";

  char* lineptr= NULL;
  std::cout<<"\nRest of Message(unparsed):\n";
  while(getline(&lineptr, &sz, msg_stream) > 0){
    std::cout<<lineptr;
    if(lineptr[0]=='\r' && lineptr[1]=='\n'){
      break;
    }
    
  }

  
  
  

  return EXIT_SUCCESS;

}
