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
#include <pthread.h>
#include "ConnObj.h"


void socket_setup(int& server_socket){

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

  freeaddrinfo(host_info_list);
 



}

void get_connections(int& server_socket, ConnObj* conn_state){
  
  struct sockaddr_in client_addr;
  

  socklen_t client_addr_size;
  client_addr_size = sizeof(client_addr); 
  
  std::cout<<"waiting for connection\n";
  conn_state->response_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
  std::cout<<"Connection received\n";
  if(conn_state->response_socket < 0){
    fprintf(stderr, "ERROR ACCEPTING");
  }


}


void* parse(void* conn_state_void){

  ConnObj* conn_state = (ConnObj*)conn_state_void;
  FILE* msg_stream = conn_state->msg_stream;
  size_t sz;
  char* request_method =NULL;
  char* request_URI = NULL;
  char* http_version = NULL;
  char * ptr = NULL;
  


  if(getdelim(&request_method, &sz, ' ', msg_stream) > 0){
    ptr = strchr(request_method, ' ');
    *ptr = '\0';
  }
  
 
  if(getdelim(&request_URI, &sz, ' ', msg_stream) > 0){
    ptr = strchr(request_URI, ' ');
    *ptr = '\0';
  }
   
  if(getdelim(&http_version, &sz, '\n', msg_stream) > 0){
    ptr = strchr(http_version, '\n');
    *ptr = '\0';
  }

  Request* req = new Request(request_method, request_URI, http_version);

  //  req->request_method = std::string(request_method);
  //req->request_URI = std::string(request_URI);
  //req->http_version = std::string(http_version);
  


  std::cout<< "Request Type: "<< req->request_method << "\n"; 
  std::cout<<"Request URI: "<< req->request_URI <<"\n";
  std::cout<<"HTTP Version: "<< req->http_version <<"\n";
  

  char* lineptr= NULL;
  std::cout<<"\nRest of Message(unparsed):\n";
  while(getline(&lineptr, &sz, msg_stream) > 0){
    std::cout<<lineptr;
    if(lineptr[0]=='\r' && lineptr[1]=='\n'){
      break;
    
      
     
    }
   
  }

  free(request_method);
  free(request_URI);
  free(http_version);
  free(lineptr);
  fclose(conn_state->msg_stream); 
  delete conn_state;
  
  std::cout<<"Killing this thread!\n";
  
  pthread_exit(NULL);
  return NULL;
  
}



int main(int argc, char ** argv) {

 
  int server_socket;
  int response_socket[5];
  
  int i = 0;

  socket_setup(server_socket);

  while(1){
    
    ConnObj* conn_state = new ConnObj();
    pthread_t thread_name;

    get_connections(server_socket, conn_state);
    
    conn_state->msg_stream = fdopen(conn_state->response_socket, "r");
        

    if (conn_state->msg_stream == NULL){
      fprintf(stderr, "Error: %s\n",strerror(errno)); 
    
    }

    int spawn = pthread_create(&thread_name, NULL, parse, conn_state);
    pthread_detach(thread_name);

    if(spawn){
      fprintf(stderr, "Error spawning new thread, returned: %d\n", spawn);
    }
   
    i++;
  
  }

  std::string buffer  = "<http>hello</http>";
  send(response_socket[0], buffer.c_str(), 6,0 );
  
  

  return EXIT_SUCCESS;

}
