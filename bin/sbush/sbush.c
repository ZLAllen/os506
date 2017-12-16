#include <stdlib.h>
#include <stdio.h>
#include <syscall.h>
#include <dirent.h>
#include <fcntl.h>

int main(int argc, char *argv[], char *envp[])
{
   __asm__ volatile ("movq $50, %rax");
   __asm__ volatile ("movq $60, %rbx");

  __asm__ volatile ("int $0x80");

  test(7000);
  //yield(); // lol that'd be too easy...


  //test cases


       
  //printf("WILL START TESTING\n");
  
  struct dstream *dirp;
  //struct linux_dirent *drent;
  char *path = "hello";
   
  
  //printf("start openddir\n");  
  dirp = opendir(path); 
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
  closedir(dirp);
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
  while(1);

  return 0;

}
