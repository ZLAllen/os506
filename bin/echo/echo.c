#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

#define BLOC 512
int main(int argc, char* argv[], char* envp[]){
    printf("%s\n", argv[1]);

   if(argc >= 2){
      for(int i = 1; i < argc; i++){
          if (argv[i])
              printf("%s ", argv[i]);
      }
   }
   
   exit();
}
