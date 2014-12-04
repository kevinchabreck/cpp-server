// #include <cstdlib>
#include <unistd.h>
#include <iostream>

void runTests(){
  int pid = fork();
  std::cout << pid << std::endl;
  if (pid==0) {
    std::cout<<"Executing command ./server\n";
    execl("./server", (char *) 0);
  }
}

int main(int argc, char ** argv) {
  runTests();
  return EXIT_SUCCESS;
}