#include <stdlib.h>
#include <stdio.h>
#include <syscall.h>

void _start(void) {
    char** argv, **envp;
    int* argc;
    extern char** environ;
    

    __asm__ volatile("movq %%rsp, %%rax\n"
            :"=a"(argc)
         );

    argv = (char**)((char*)argc + 24);
    argc = argc + 6;

     envp = &(argv[*argc + 1]);
     environ = envp;

     if(!*environ){
         printf("no environment variables\n");
     }

    // call main() and exit() here
     main(*argc,argv,envp);
    
     printf("main returned\n");

      exit();
}
