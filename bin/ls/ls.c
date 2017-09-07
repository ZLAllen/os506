#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <syscalls.h>

char *getname(char *path)
{
    char *fname;

    for(fname=path+strlen(path); fname >= path && *fname != '/'; fname--);
    fname++; // remove the slash

    return fname;
}

void ls(char *path)
{
    char buf[512];
    int fd, nread, bpos;
    struct linux_dirent *d;

    fd = open(path, 0);

    if((fd) == -1){
        puts("cannot open the file");
        return;
    }

    for(;;){
        nread = getdents(fd, (struct linux_dirent *)buf, 512);
        if(nread == -1){
            puts("panic: getdents");
            exit(1);
        }

        if(nread == 0)
            break;


        for(bpos=0; bpos < nread;){
            d= (struct linux_dirent*)(buf + bpos);
            puts(d->d_name);
            bpos += d->d_reclen;
        }

    }
    close(fd);
}

int main(int argc, char *argv[], char* envp[])
{

    int i;

    if(argc < 2){
        // get the current directory and then call ls
        ls(".");
        exit(0);
    }
    // call on the given path
    for(i=1; i<argc; i++)
        ls(argv[i]);
    exit(0);


}
