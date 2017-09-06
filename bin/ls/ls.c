#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>

#define SZ 50


char *getname(char *path)
{
    char *fname;

    for(fname=path+strlen(path); fname >= path && *fname != '/'; fname--);
    fname++; // remove the slash

    return fname;
}

void ls(char *path)
{
    char buf[512], *p;
    int fd;
    struct dirent drnt;
    struct stat st;

    fd = open(path, 0);

    if((fd) == -1){
        printf("cannot open the file%s\n", path);
        return;
    }

    if(fstat(fd, &st) == -1){
        printf("cannot stat the file%s\n", path);
        close(fd);
        return;
    }

    int mode = S_ISDIR(st.st_mode);
    switch(mode){
        case 0:
            printf("%s %d %ld %ld\n", getname(path), mode, st.st_ino, st.st_blksize);
            break;

        case 1:
            strcpy(buf, path);
            p = buf+strlen(buf);
            *p++ = '/';
            while(read(fd, &drnt, sizeof(drnt)) == sizeof(drnt)){
                if(drnt.d_ino == 0)
                    continue;
                memmove(p, drnt.d_name, SZ);
                p[SZ] = 0;
                if(stat(buf, &st) == -1){
                    printf("cannot stat the file%s\n", buf);
                    continue;
                }
                printf("%s %d %ld %ld\n", getname(buf), mode, st.st_ino, st.st_blksize);
            }
            break;
    }
    close(fd);
}

int main(int argc, char *argv[])
{

    int i;
    char *dir;
    char cwd[1024];

    if(argc < 2){
        // get the current directory and then call ls
        dir = getcwd(cwd, sizeof(cwd));
        ls(dir);
        exit(EXIT_SUCCESS);
    }
    // call on the given path
    for(i=1; i<argc; i++)
        ls(argv[i]);
    exit(EXIT_SUCCESS);



}