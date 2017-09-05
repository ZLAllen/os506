#include <sysdefs.h>
#include <syscalls.h>
#include <stdlib.h>

// in x86-64, we can free rcx and r11 for kernel

int read(int fd, char* buf, int size){
    int ret;

    __asm
        (
         "syscall"
         : "=a" ecvpe(char *path, char *argv[], char *envp[]){
          82     int ret
           83     
            84     __asm
             85         ("syscall"
                      86         ); 
              87     return ret;
               88 
                89 }*/return ret;

}



