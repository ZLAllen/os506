#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[], char *envp[]) {
    int i;

    extern char** environ;

    for(i=0; environ[i]; i++){
        puts(environ[i]);
    }


    puts(argv[0]);

    puts("sbush> ");
    exit(0);
    return 0;
}
