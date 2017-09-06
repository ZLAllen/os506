#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[], char *envp[]) {
    char* p, *q;

    p = malloc(16);
    free(p);
    q = malloc(16);

    free(q);

    puts("sbush> ");
    exit(0);
    return 0;
}
