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

	//printf("argc from: %x, value: %d\n", argc, *argc);
	//printf("argv from: %x but currently from: %x\n", argc+8, argv);

    envp = &(argv[*argc + 1]);

	//printf("envp from: %x, value: %d\n", argc+8, *envp);
    
	environ = envp;
    if(!*environ){
         printf("no environment variables\n");
     }

    // call main() and exit() here
	printf("argc: %d, argv: %s, envp: %s\n", *argc, argv, envp);


    main(*argc, argv, envp);
    
    printf("main returned\n");

    exit();
}
