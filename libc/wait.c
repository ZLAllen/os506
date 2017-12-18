#include <stdlib.h>
#include <syscall.h>

int wait(int *status){
    int ret;

    ret = waitpid(-1, status);

    return ret;

}
