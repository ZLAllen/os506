#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/*
 *it finds the last name after "/" and has no trailig slash
 * */
char *getname(char *path)
{
    char *fname;

    for(fname=path+strlen(path); fname >= path && *fname != '/'; fname--);
    fname++; // remove the slash

    return fname;
}	

/*
 * eg: bin/ or bin/sbush/
 * */
void ls(char *path)
{	

    char buf[1024];
    int fd, nread, bpos;
    struct linux_dirent *d;

    fd = open("bin/usr", 0);

    if((fd) == -1){
        puts("cannot open the file\n");
        return;
    }

    for(;;)
	{
        nread = getdents(fd, (struct linux_dirent *)buf, 1024);
        if(nread == -1){
            puts("panic: getdents");
            exit(1);
        }

        if(nread == 0)
            break;


        for(bpos=0; bpos < nread;){
            d= (struct linux_dirent*)(buf + bpos);
            printf("%s\n", d->d_name);
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
