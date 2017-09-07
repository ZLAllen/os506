#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#define BLOC 512

void cat(int fd){
    int nread;
    char buf[BLOC];
    // 512 bytes per chunk
    while((nread = read(fd, buf, BLOC)) > 0){
        if(write(1, buf, nread) < 0){
            puts("write data failed");
            exit(1);
        }
    }
    if(nread < 0){
        puts("read data failed");
        exit(1);
    }
}


int main(int argc, char* argv[], char* envp[]){
    int fd;

   //if no file provide, read from stdin
   if(argc < 2){
       cat(0);
   }else{
      // if multiple files here, read one by one
      for(int i = 1; i < argc; i++){
          // need to implement flags for open
          fd=open(argv[i], O_RDONLY);
          if(fd < 0){
              puts("bad fd\n");
              exit(1);
              //error and exit
          }
          cat(fd);
          close(fd);
      }
   }
   
   exit(0);
}
