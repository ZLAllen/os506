#include <stdlib.h>
#include <stdio.h>

void _start(void) {
    char** argv, **envp, **environ;
    int* argc;
    int ret;
    __asm("movq %%rsp, %%rax\n"
          "addq $24, %%rax"
            :"=a"(argc)
         );

    argv = (char**)((char*)argc + 8);

     envp = &(argv[*argc + 1]);
     environ = envp;

     if(!*environ){
         exit(1);
     }

    // call main() and exit() here
    ret = main(*argc,argv,envp);
    exit(ret);
}
