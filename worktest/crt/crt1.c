#include <stdlib.h>
#include <stdio.h>

void _start(void) {
    char** argv, **envp;
    int* argc;
    __asm("movq %%rsp, %%rax\n"
          "addq $8, %%rax"
            :"=a"(argc)
         );

    argv = (char**)((char*)argc + 8);

    envp = &(argv[*argc + 1]);

    // call main() and exit() here
    int ret = main(*argc,argv,envp);
    exit(ret);
}
