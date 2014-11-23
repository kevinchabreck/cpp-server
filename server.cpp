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



// TODO: make sure to delete req and conn_state

void socket_setup(int& server_socket){
  int status;
  struct addrinfo host_info;
  struct addrinfo* host_info_list;
  memset(&host_info, 0 , sizeof(host_info));
  host_info.ai_family = AF_INET;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags = AI_PASSIVE;
  status = getaddrinfo(NULL, "8000", &host_info, &host_info_list);
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

Request* parse(ConnObj* conn_state){
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
  char* lineptr = NULL;
  
  while (getline(&lineptr, &sz, msg_stream) > 0){
    if(lineptr[0]=='\r' && lineptr[1]=='\n'){
      break;
    }
    ptr = strchr(lineptr, '\n');
    *ptr = '\0';
    ptr = strchr(lineptr, ':');
    *ptr = '\0';
    ptr = ptr + 2;
    req->addHeader(lineptr, ptr); 
  }
  if(req->request_method == "POST" || req->request_method == "PUT"){
    getline(&lineptr, &sz, msg_stream);
    req->addBody(lineptr);
  }
  free(request_method);
  free(request_URI);
  free(http_version);
  free(lineptr);  
  return req;
}

int callFunc(std::string request_method, Request * req, ConnObj* conn_state){
  int success;

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

//   else if(request_method == "PUT"){
//     std::cout<<"This is a PUT!\n";
//     putResponse(req, conn_state);
//     success = 1; 
//   }

// else if(request_method == "HEAD"){
//     std::cout<<"This is a HEAD!\n";
//     headResponse(req, conn_state);
//     success = 1; 
//   }

// else if(request_method == "OPTIONS"){
//     std::cout<<"This is a OPTIONS!\n";
//     optionsResponse(req, conn_state);
//     success = 1; 
//   }

// else if(request_method == "DELETE"){
//     std::cout<<"This is a DELETE!\n";
//     deleteResponse(req, conn_state);
//     success = 1; 
//   }

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
  ConnObj* conn_state = (ConnObj*)conn_state_void;
  Request* req;
  std::string persist = std::string("keep-alive");
  struct timeval hang_out_time;
  // double chillax = .2;
  double chillax = 500000;
  hang_out_time.tv_sec = 0;
  hang_out_time.tv_usec = chillax;
  fd_set set;
  FD_ZERO(&set);
  FD_SET(conn_state->response_socket, &set);
  double i = 0;
  int available = 0;

  while(i < 1){
  // while(i < 2){
    std::cout<<"LOOPING - i = " << i <<"\n";
    available = select(FD_SETSIZE, &set, NULL, NULL, &hang_out_time);
    hang_out_time.tv_usec = chillax;
    if(available > 0){
      req = parse(conn_state);
      if((req->headers).count("connection")){
        if(req->headers["connection"].find(persist) == std::string::npos){
          chillax = 0;
        }
      }
      else{
        chillax = 0;
      }
      int success = callFunc(req->request_method, req, conn_state);
      if(success == 1){
        req->printRequest();
      } 
      else{
        std::cout<<"Bad request";
      }
      i = 0;
    }
    else{
      i+=chillax/1000000;
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
  socket_setup(server_socket);

  while(1){
    ConnObj* conn_state = new ConnObj();
    pthread_t thread_name;
    get_connections(server_socket, conn_state);
    conn_state->msg_stream = fdopen(conn_state->response_socket, "r");
    if (conn_state->msg_stream == NULL){
      fprintf(stderr, "Error: %s\n",strerror(errno)); 
    }
    int spawn = pthread_create(&thread_name, NULL, handle, conn_state);
    pthread_detach(thread_name);
    if(spawn){
      fprintf(stderr, "Error spawning new thread, returned: %d\n", spawn);
    }
    i++;
  }

  return EXIT_SUCCESS;
}
