#include <stdlib.h>
#include <stdio.h>
#include <syscall.h>
#include <dirent.h>
#include <fcntl.h>

int main(int argc, char *argv[], char *envp[])
{
  // __asm__ volatile ("movq $50, %rax");
  // __asm__ volatile ("movq $60, %rbx");

  //__asm__ volatile ("int $0x80");
/*
  test(7000);
  yield(); // lol that'd be too easy...
*/

  //test cases
  /*  
  char* msg = "usr/next_hello";
  int fd = open(msg, 0);
  if(fd < 0)
  {
    write(1, "a", 1);
    while(1);
  }*/


/*	
  char a[10];
  if(read(0, a, 10) < 0)
  {
    write(1, "b", 1);
    while(1);
  }

  if(write(1, a, 10) < 0)
  {
  } 


  	
  int *addr = (int *)0xF000000UL;

  //testing sucess cond
  brk(addr); 

  //testing failure cond
  addr = (int *) -1;
  brk(addr);*/

  //while(1);
  
  char *file1 = "hello";
  char *args1[] = {"hello", "arg1", "arg2", NULL};
  char *envs1[] = {"key=value", NULL};
  
  execve(file1, args1, envs1);
  /*if(ret < 0)
	  while(1);*/

  
  char *file2 = "/bin/ls";

  execve(file2, NULL, NULL);
  /*if(ret < 0)
	  while(1);*/


  //while(1)
  //char *args[] = {"/bin/ls", "-lR", NULL};
  //execve(args[0], args, NULL);

  //this should definitely give segfault
  //*addr = 0x1234;

  //printf("WILL START TESTING\n");
  
  //struct dstream *dirp;
  //struct linux_dirent *drent;
  //char *path = "usr/next_hello";
   
  
  //printf("start openddir\n");  
  //opendir(path); 
  //printf("done opendir\n");
  //while(1);	
    
  //printf("start readdir\n");
  /*  
  int num = 0;
  while((drent = readdir(dirp)) != NULL) 
  {
	//printf("reading file: %s\n", drent->d_name);
	num ++;
  }
  //printf("done readdir\n");
  */
  //printf("start closedir");
 // closedir(dirp);
  //printf("done closedir");

  /*
  printf("start open");
  int fd = open("", O_RDONLY);  
  printf("done open");

  printf("start read");
  char* buff = (char *)malloc(512);
  int size = read(fd, buff, 25);
  printf("read: %p", size);
  printf("done read");

  printf("start close");
  int ret = close(fd);
  if(ret == 0)
  	printf("done close");

  free(buff);
  */


  //while(1);
  //printf("just wanna test opendir\n");
  //opendir("/rootfs");
  
  yield();
  exit();

  while(1);

  return 0;

}
