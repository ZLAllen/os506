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
      printf("nread: %d\n", nread);
      if(write(1, buf, nread) < 0){
            printf("write data failed\n");
            exit(1);
        }
    }

    printf("nread: %d\n", nread);
    if(nread < 0){
        printf("read data failed\n");
        exit(1);
    }
}


int main(int argc, char* argv[], char* envp[]){
    int fd;

   //if no file provide, read from stdin
   if(argc < 1){
       cat(0);
   }else{
      // if multiple files here, read one by one
      for(int i = 0; i < argc; i++){
          // need to implement flags for open
          printf("argv[0]: %s\n", argv[0]);
          fd=open(argv[i], O_RDONLY);
          if(fd < 0){
              printf("bad fd\n");
              exit();
              //error and exit
          }
          cat(fd);
          close(fd);
      }
   }
   
   exit();
}
