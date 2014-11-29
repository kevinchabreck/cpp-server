#include <sys/select.h>
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ConnObj.h"
#include "response.h"
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include "putResponse.h"
#include <sys/ioctl.h>
#include "delete.h"
#include "head.h"
#include "options.h"


// TODO: make sure to delete req and conn_state

void socket_setup(int& server_socket){   //This method sets up and binds socket to port 8000
  int status;
  struct addrinfo host_info;    //These structs are populated by getaddrinfo function
  struct addrinfo* host_info_list;
  memset(&host_info, 0 , sizeof(host_info)); //Initalizing to 0
  host_info.ai_family = AF_INET;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags = AI_PASSIVE;
  status = getaddrinfo(NULL, "8000", &host_info, &host_info_list); //Populating the structs
  if(status < 0){
    fprintf(stderr, "error getting host info"); 
  }
   
  server_socket = socket (AF_INET, SOCK_STREAM, 0); //Create TCP socket
  if (server_socket < 0){
    fprintf(stderr, "COULD NOT OPEN SOCKET");
  }

  int yes = 1;
  setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

  if (bind(server_socket, host_info_list->ai_addr, host_info_list->ai_addrlen) < 0){ //Bind socket
    fprintf(stderr,"BINDING FAILED: %d", errno);
    std::cout<<std::strerror(errno)<<"\n";
  }

  if(listen(server_socket, 5)< 0){ //Queue socket to start listening
    fprintf(stderr, "Listening Failed");
  }

  freeaddrinfo(host_info_list); //Free the struct used earlier
}

void get_connections(int& server_socket, ConnObj* conn_state){
  struct sockaddr_in client_addr; //this struct used by accept method
  socklen_t client_addr_size;
  client_addr_size = sizeof(client_addr); 
  
  std::cout<<"waiting for connection\n";
  conn_state->response_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size); //This is a blocking method that waits for a connection to proceed
  std::cout<<"Connection received\n";
  if(conn_state->response_socket < 0){
    fprintf(stderr, "ERROR ACCEPTING");
  }
}

Request* parse(ConnObj* conn_state){ //This function parses HTTP request, excluding the entity body
  FILE* msg_stream = conn_state->msg_stream;
  size_t sz;
  char* request_method =NULL;
  char* request_URI = NULL;
  char* http_version = NULL;
  char * ptr = NULL;
  
  if(getdelim(&request_method, &sz, ' ', msg_stream) > 0){ //Get the request_method
    std::cout<<request_method;
    ptr = strchr(request_method, ' ');
    *ptr = '\0';
  }
  
  if(getdelim(&request_URI, &sz, ' ', msg_stream) > 0){ //Get the URI
    std::cout<<request_URI;
    ptr = strchr(request_URI, ' ');
    *ptr = '\0';
  }
   
  if(getdelim(&http_version, &sz, '\n', msg_stream) > 0){ //Get the HTTP version
    std::cout<<http_version;
    ptr = strchr(http_version, '\n');
    *ptr = '\0';
  }

  Request* req = new Request(request_method, request_URI, http_version); //Create a new request object based on this information
  char* lineptr = NULL;
  
  while (getline(&lineptr, &sz, msg_stream) > 0){ //Get all the optional headers present
    std::cout<<lineptr;
    if(lineptr[0]=='\r' && lineptr[1]=='\n'){ // CRLF indicates end of header fields
      break;
    }
    ptr = strchr(lineptr, '\n');
    *ptr = '\0';
    ptr = strchr(lineptr, ':');
    *ptr = '\0';
    ptr = ptr + 2;
    req->addHeader(lineptr, ptr); 
  }
  //Commented this out so body parsing can happen in message handling functions
  //  if(req->request_method == "POST" || req->request_method == "PUT"){
  //getline(&lineptr, &sz, msg_stream);
  // std::cout<<lineptr;
  // req->addBody(lineptr);
  // }
  free(request_method); //Free malloc'd memory
  free(request_URI);
  free(http_version);
  free(lineptr);  
  return req;
}

int callFunc(std::string request_method, Request * req, ConnObj* conn_state){
  int success;

  //Depending on what request type is, call appropriate function to handle it

  if(request_method =="GET"){  
    std::cout<<"This is a GET!\n";
    getResponse(req, conn_state);
    success = 1; 
  }
  
//   else if(request_method == "POST"){
//     std::cout<<"This is a POST!\n";
//     postResponse(req, conn_state);
//     success = 1; 
//   }

  else if(request_method == "PUT"){
    std::cout<<"This is a PUT!\n";
    putResponse(req, conn_state);
    success = 1; 
  }

else if(request_method == "HEAD"){
    std::cout<<"This is a HEAD!\n";
    headResponse(req, conn_state);
    success = 1; 
  }

 else if(request_method == "OPTIONS"){
     std::cout<<"This is a OPTIONS!\n";
     optionsResponse(req, conn_state);
     success = 1; 
   }

else if(request_method == "DELETE"){
    std::cout<<"This is a DELETE!\n";
    deleteResponse(req, conn_state);
    success = 1; 
  }

// else if(request_method == "TRACE"){
//     std::cout<<"This is a TRACE!\n";
//     traceResponse(req, conn_state);
//     success = 1; 
//   }

//  else if(request_method == "CONNECT"){
//    std::cout<<"This is a CONNECT!\n";
//    connectResponse(req, conn_state);
//    success = 1; 
//   }

  else{
    success = 0; 
  }
  return success;
}


void* handle(void* conn_state_void){
  //This function is what handles individual connections as they are reveived
  ConnObj* conn_state = (ConnObj*)conn_state_void;
  Request* req;
  std::string persist = std::string("keep-alive");
  struct timeval hang_out_time; //Struct to determine blockng time
  double chillax = 500000; //Watiting 0.5 seconds, 500,000 microseconds
  hang_out_time.tv_sec = 0;
  hang_out_time.tv_usec = chillax;
 

  double i = 0; 
  int available = 0;

  while(i < 1){
    std::cout<<"LOOPING - i = " << i <<"\n";
    
    fd_set set; //These structs and the select() method determine if there is something to be read in the socket
    FD_ZERO(&set); 
    FD_SET(conn_state->response_socket, &set);
    available = select(FD_SETSIZE, &set, NULL, NULL, &hang_out_time);
    
    //count will return number of bytes available to read from socket
    int count;
    ioctl(conn_state->response_socket, FIONREAD, &count);
  
    hang_out_time.tv_usec = chillax;
   
    if(available > 0 && count > 0){ //If there is data available to read from socket, enter the loop
      req = parse(conn_state); //Parse the socket data
     
      if((req->headers).count("connection")){ //If no connection:keep alive stated, set wait time to 0 so will kill connection quickly
        if(req->headers["connection"].find(persist) == std::string::npos){
          chillax = 0;
        }
      }
      else{
        chillax = 0;
      }
      std::cout<<"Calling a function...\n"; //Call callFunc to call appropriate msg handler
      int success = callFunc(req->request_method, req, conn_state);
      if(success == 1){
        req->printRequest();
	i = 0;
      } 
      else{
        std::cout<<"Bad request";
	//INSERT CALL TO BAD REQUEST FuNCTION
	i = 2;
      }
     
    }
    else{
      i+=.5;
    }
  }
  std::cout<<"Killing this thread!\n";
  fclose(conn_state->msg_stream); 
  delete(conn_state);
  pthread_exit(NULL);
  return NULL;
}

int main(int argc, char ** argv) {
  int server_socket;
  int i = 0;
  socket_setup(server_socket); //Set up the socket

  while(1){ //This loop goes infinitely on listening for new connections
    ConnObj* conn_state = new ConnObj();
    pthread_t thread_name;
    get_connections(server_socket, conn_state);
    conn_state->msg_stream = fdopen(conn_state->response_socket, "r");
    if (conn_state->msg_stream == NULL){
      fprintf(stderr, "Error: %s\n",strerror(errno)); 
    }
    int spawn = pthread_create(&thread_name, NULL, handle, conn_state); //Spawn new thread when get new connection
    pthread_detach(thread_name); //Detach from thread to enable freeing of the memory
    if(spawn){
      fprintf(stderr, "Error spawning new thread, returned: %d\n", spawn);
    }
    i++;
  }

  return EXIT_SUCCESS;
}
