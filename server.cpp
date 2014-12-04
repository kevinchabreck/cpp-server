#include <iostream>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include "connection.h"
#include "get.h"
#include "put.h"
#include "head.h"
#include "delete.h"
#include "options.h"
#include "post.h"
#include "common.h"

int mode;

//This method sets up and binds socket to port 8000
void socket_setup(int& server_socket){
  int status;
  struct addrinfo host_info;    //These structs are populated by getaddrinfo function
  struct addrinfo* host_info_list;
  memset(&host_info, 0 , sizeof(host_info)); //Initalizing to 0
  host_info.ai_family = AF_INET;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags = AI_PASSIVE;
  status = getaddrinfo(NULL, "8080", &host_info, &host_info_list); //Populating the structs
  if(status < 0){
    if (mode == DEBUG){
      log("Error Getting Host Info");
    }
    exit(EXIT_FAILURE);
  }
   
  server_socket = socket (AF_INET, SOCK_STREAM, 0); //Create TCP socket
  if (server_socket < 0){
    if (mode == DEBUG){
      log("Error Creating Socket");
    }
    exit(EXIT_FAILURE);
  }

  int yes = 1;
  setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

  if (bind(server_socket, host_info_list->ai_addr, host_info_list->ai_addrlen) < 0){ //Bind socket
    if (mode == DEBUG){
      log("Binding Socket to Port Failed");
    }
    exit(EXIT_FAILURE);
  }

  if(listen(server_socket, 5)< 0){ //Queue socket to start listening
    if (mode == DEBUG){
      log("Listening to Socket Failed");
    }
    exit(EXIT_FAILURE);
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
    if (mode == DEBUG){
      log("Accepting Connection Failed");
    }
    exit(EXIT_FAILURE);
  }
}

//This function parses HTTP request, excluding the entity body
Request* parse(ConnObj* conn_state){
  FILE* msg_stream = conn_state->msg_stream;
  size_t sz;
  char* request_method =NULL;
  char* request_URI = NULL;
  char* http_version = NULL;
  char * ptr = NULL;
  
  if(getdelim(&request_method, &sz, ' ', msg_stream) > 0){ //Get the request_method
    log("Message Received: " + request_method);
    ptr = strchr(request_method, ' ');
    *ptr = '\0';
  }
  
  if(getdelim(&request_URI, &sz, ' ', msg_stream) > 0){ //Get the URI
    log(request_URI);
    ptr = strchr(request_URI, ' ');
    *ptr = '\0';
  }
   
  if(getdelim(&http_version, &sz, '\n', msg_stream) > 0){ //Get the HTTP version
    log(http_version);
    ptr = strchr(http_version, '\n');
    *ptr = '\0';
  }

  Request* req = new Request(request_method, request_URI, http_version); //Create a new request object based on this information
  char* lineptr = NULL;
  char * cr = NULL;
  char* lf = NULL;
  char* test = NULL;
  while (getline(&lineptr, &sz, msg_stream) > 0){ //Get all the optional headers present
    
    if (mode == DEBUG){
      log(std::string(lineptr);
    }
   
    if(lineptr[0]=='\r' && lineptr[1]=='\n'){ // line with only CRLF indicates end of header fields
      break;
    }
    
    lf = strchr(lineptr, '\n'); //Removing carriage return/new lines in header fields
    if(lf != NULL){
      *lf = '\0';
    }

    cr = strchr(lineptr, '\r');
    if(cr != NULL){
      *cr = '\0';
    }

    ptr = strchr(lineptr, ':');
    *ptr = '\0';
    ptr = ptr + 2;
    req->addHeader(lineptr, ptr); 
  }

  free(request_method); //Free malloc'd memory
  free(request_URI);
  free(http_version);
  free(lineptr);  
  return req;
}

int callFunc(std::string request_method, Request * req, ConnObj* conn_state){
  int success;

  //Depending on what request type is, call appropriate function to handle it

  // TODO: convert this to a switch statement
  if(request_method =="GET"){  
    getResponse(req, conn_state);
    success = 1; 
  }
  
  else if(request_method == "POST"){
    postResponse(req, conn_state);
    success = 1; 
  }

  else if(request_method == "PUT"){
    putResponse(req, conn_state);
    success = 1; 
  }

  else if(request_method == "HEAD"){
    headResponse(req, conn_state);
    success = 1; 
  }

  else if(request_method == "OPTIONS"){
    optionsResponse(req, conn_state);
    success = 1;
  }

  else if(request_method == "DELETE"){
    deleteResponse(req, conn_state);
    success = 1; 
  }

  //Not supporting these two methods, send 501
  else if(request_method == "TRACE"){
    log("Request Not Fulfilled, Response Sent (Method not Implemented)");
    send501(conn_state);
    success = 1;
  }

  //Not supporting these two methods, send 501
  else if(request_method == "CONNECT"){
    log("Request Not Fulfilled, Response Sent (Method not Implemented)");
    send501(conn_state);
    success = 1; 
  }
 
  else{
    success = 0; 
  }
  return success;
}


void* handle(void* conn_state_void){
  //This function is what handles individual connections as they are received
  ConnObj* conn_state = (ConnObj*)conn_state_void;
  Request* req;
  std::string persist = std::string("keep-alive");
  struct timeval hang_out_time; //Struct to determine blocking time
  double chillax = 500000; //Watiting 0.5 seconds, 500,000 microseconds
  hang_out_time.tv_sec = 0;
  hang_out_time.tv_usec = chillax;

  double i = 0; 
  int available = 0;

  while(i < 1){
    // determine if there is something to be read in the socket
    fd_set set;
    FD_ZERO(&set); 
    FD_SET(conn_state->response_socket, &set);
    available = select(FD_SETSIZE, &set, NULL, NULL, &hang_out_time);
    //count will return number of bytes available to read from socket
    int count;
    ioctl(conn_state->response_socket, FIONREAD, &count);
    hang_out_time.tv_usec = chillax;
    //If there is data available to read from socket, enter the loop
    if(available > 0 && count > 0){
      req = parse(conn_state); //Parse the socket data
     
      if((req->headers).count("connection")){
        //check for "connection:keep alive"
        if(req->headers["connection"].find(persist) == std::string::npos){
          chillax = 0;
        }
      }
      else{
        chillax = 0;
      }
      //Call callFunc to call appropriate msg handler
      int success = callFunc(req->request_method, req, conn_state);
      if(success == 1){
        i = 0;
      } 
      else{
        log("Message Not Understood, Status Code 400 (Bad Request) Sent");
        send400(conn_state);
        delete req;
        break;
      }
      delete req;
      req = NULL;
    }
    else{
      i+=.5;
    }
  }
  
  if(mode == DEBUG){
    log("Connection Being Terminated");
  }
  fclose(conn_state->msg_stream); 
  delete(conn_state);
  pthread_exit(NULL);
  return NULL;
}

int main(int argc, char ** argv) {
  if (argc != 2) {
    std::cout<<"usage:\n";
    std::cout<<"    server start    (launches in background as a daemon)\n";
    std::cout<<"    server stop     (stop server daemon)\n";
    std::cout<<"    server debug    (launch server in foreground)\n";
    return EXIT_FAILURE;
  }
  std::string m = argv[1]; 
  if(m == "start"){
    mode = STANDARD;
    struct stat exists;
    if(stat("logs", &exists)){
      std::cout<<"no logs directory - creating one\n";
      mkdir("logs", S_IRWXU | S_IRWXG);
    }
  }
  else if (m == "stop"){
    return EXIT_SUCCESS;
  }
  else if (m == "debug" || m == "d"){
    mode = DEBUG;
  }

  //Set up the socket
  int server_socket;
  socket_setup(server_socket);
  ConnObj* conn_state = NULL;
  // listen for new connections
  while(1){
    conn_state = new ConnObj();
    pthread_t thread_name;
    get_connections(server_socket, conn_state);
    conn_state->msg_stream = fdopen(conn_state->response_socket, "r");
    if (conn_state->msg_stream == NULL){
      fprintf(stderr, "Error: %s\n",strerror(errno)); 
    }
    //Spawn new thread when get new connection
    int spawn = pthread_create(&thread_name, NULL, handle, conn_state);
    //Detach from thread to enable freeing of the memory
    pthread_detach(thread_name);
    if(spawn){
      fprintf(stderr, "Error spawning new thread, returned: %d\n", spawn);
    }
  }
  
  return EXIT_SUCCESS;
}
