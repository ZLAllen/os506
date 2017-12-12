#include <stdlib.h>
#include <syscall.h>

int wait(int *status){
    int ret;

    ret = wait4(-1, status, 0);

    return ret;

}
