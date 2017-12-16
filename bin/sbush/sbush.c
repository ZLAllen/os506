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

  test(7000);
  yield(); // lol that'd be too easy...


  //test cases
  char* msg = "usr/next_hello";
  int fd = open(msg, 0);
  if(fd < 0)
  {
    write(1, "a", 1);
    while(1);
  }

  char a[10];
  if(read(fd, a, 10) < 0)
  {
    write(1, "b", 1);
    while(1);
  }
  if(write(1, a, 5) < 0)
  {
  } 
  if(read(fd, a, 10) < 0)
  {
    write(1, "b", 1);
    while(1);
  }
  if(write(1, a, 5) < 0)
  {
  } 
  while(1);
  

       
  //printf("WILL START TESTING\n");
  
  //struct dstream *dirp;
  //struct linux_dirent *drent;
  //char *path = "usr/next_hello";
  //
  
   
  
  //printf("start openddir\n");  
 // dirp = opendir(path); 
  //printf("done opendir\n");

    
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
 // printf("just wanna test opendir\n");
  //opendir("/rootfs");
  yield();
  exit();
  while(1);

  return 0;

}
