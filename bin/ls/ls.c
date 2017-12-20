#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

char *getname(char *path)
{
    char *fname;

    for(fname=path+strlen(path); fname >= path && *fname != '/'; fname--);
    fname++; // remove the slash

    return fname;
}

void ls(int fd)//(char *path)
{
    char buf[512];
    //int fd;
	int nread, bpos;
    struct linux_dirent *d;

    //fd = open(path, O_RDONLY);

    if((fd) == -1){
        puts("cannot open the file\n");
        return;
    }

    for(;;){
        nread = getdents(fd, (struct linux_dirent *)buf, 512);
		printf("nread: %d\n", nread);
        if(nread == -1){
            puts("panic: getdents\n");
            exit(1);
        }

        if(nread == 0)
            break;

		if(nread == -5)
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
	
    /*int i;
	
	//hard coding it now as arg is messed up somewhere
	ls("bin/");


    if(argc < 2){
        // get the current directory and then call ls
        ls(".");
        exit(0);
    }

    // call on the given path
    for(i=1; i<argc; i++)
        ls(argv[i]);
    exit(0);*/

   int fd;

   if(argc < 1){
       ls(0);
   }else{
      // if multiple files here, read one by one
      for(int i = 0; i < argc; i++){
          // need to implement flags for open
          printf("argv[0]: %s\n", argv[0]);
          fd=open(argv[i], O_RDONLY);
          if(fd < 0){
              printf("bad fd\n");
              exit();
              //error and exit
          }
		  ls(fd);
	  }
   }
	exit();

}
