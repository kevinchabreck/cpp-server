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

int main(int argc, char ** argv) {

  const int port_number = 80;
  int client_addr_size;
  int server_socket;
  int response_socket;
  int read_length;
  
  
  char buffer [256];

  struct sockaddr_in server_addr, client_addr;
  server_socket = socket (AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0){
    fprintf(stderr, "COULD NOT OPEN SOCKET");
  }

  
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port_number);
  
  server_addr.sin_addr.s_addr = INADDR_ANY;
 
  if (bind(server_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0){
    fprintf(stderr,"BINDING FAILED");
  }

  listen(server_socket, 5);


  client_addr_size = sizeof(client_addr);

  response_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);

  if(response_socket < 0){
    fprintf(stderr, "ERROR ACCEPTING");
  }

  memset(buffer, 0, sizeof(buffer));

  read_length = read(response_socket, buffer, 255);

  if(read_length < 0) {
    fprintf(stderr, "ERROR READING SOCKET");
  }

  std::cout<< "Here is the message: \n" << buffer; 

  return EXIT_SUCCESS;

}
