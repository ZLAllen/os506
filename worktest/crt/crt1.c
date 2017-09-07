#include <stdlib.h>
#include <stdio.h>

void _start(void) {
    char** argv, **envp;
    int* argc;
    int ret;
    extern char** environ;
    __asm("movq %%rsp, %%rax\n"
          "addq $24, %%rax"
            :"=a"(argc)
         );

    argv = (char**)((char*)argc + 8);

     envp = &(argv[*argc + 1]);
     environ = envp;

     if(!environ){
         puts("no environment variables\n");
     }

    // call main() and exit() here
    ret = main(*argc,argv,envp);
    exit(ret);
}
