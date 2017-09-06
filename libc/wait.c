#include <stdlib.h>
#include <syscalls.h>

int wait(int *status){
    int ret;

    ret = wait4(-1, status, 0);

    return ret;

}
