#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[], char *envp[]) {
    int i;
    for(i=0; envp[i]; i++)
        puts(envp[i]);

    puts("sbush> ");
    exit(0);
    return 0;
}
