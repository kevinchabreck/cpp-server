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
  int read_length;
  int status;
  
  char buffer [256];

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

  memset(buffer, 0, sizeof(buffer));

  read_length = read(response_socket, buffer, 255);

  if(read_length < 0) {
    fprintf(stderr, "ERROR READING SOCKET");
  }

  if(read_length == 0){
    fprintf(stderr, "Host Lost");
}

  buffer[read_length] = '\0';

  std::cout<< "Here is the message: \n" << buffer; 

  return EXIT_SUCCESS;

}
