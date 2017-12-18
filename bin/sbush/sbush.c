#include <stdlib.h>
#include <stdio.h>
#include <syscall.h>
#include <dirent.h>
#include <fcntl.h>


int main(int argc, char *argv[], char *envp[])
{



  printf("user A\n");

  yield();

  printf("user B\n");

  yield();


  printf("user C\n");


  char a[10];

  read(0, a, 10);

  printf("user c %s\n", a);

  uint64_t pid = fork();
  if (pid == 0) {
      printf("Child\n");
      yield();
      printf("child again\n");
      yield();
      printf("child done\n");
      exit();
  } else {
      printf("Parent\n");
      
    printf("Parent will now wait for its child\n");
    int waitStatus = 9000;
    int waitRet = wait(&waitStatus);
    printf("Waiting done. Return %d. Status %d\n", waitRet, waitStatus);
      yield();
    char* msg = "usr/next_hello";  
    int fd = open(msg, 0);
    if(fd < 0)
    {
      write(1, "a", 1);
      while(1);
    }
    yield();
    printf("final parents\n");
  }


  exit();


  // __asm__ volatile ("movq $50, %rax");
  // __asm__ volatile ("movq $60, %rbx");

  //__asm__ volatile ("int $0x80");
/*
  test(7000);
  yield(); // lol that'd be too easy...
*/

  //test cases
/*
  uint64_t pid = fork();
  if (pid == 0) {
      write(1, "Child\n", 5);
  } else {
      write(1, "Parent\n", 6);
      yield();

    char* msg = "usr/next_hello";  
    int fd = open(msg, 0);
    if(fd < 0)
    {
      write(1, "a", 1);
      while(1);
    }
    yield();
    write(1, "final parents", 15);
  }
  //printf("Moo");
  yield();
  while(1);
  
  


  char* addr = (char*)brk((void*)-1);

  //testing sucess cond
  char* naddr = brk((void*)(addr + 100)); 


  //testing failure cond
  //read(fd, naddr, 100);
  //read(fd, naddr, 100);

  write(1, naddr, 100);
 

  read(0, naddr, 100);


  while(1);

*/

/*
  char* a = malloc(64);

  if(!a)
  {
    printf("nothing allocated\n");
  }


  char* b = malloc(128);
  if(!b)
  {
    printf("b allocation failed\n");
  }

  struct dstream *dirp;
  dirp = malloc(sizeof(*dirp));
  if(!dirp)
  {
	  printf("dirp allocation failed\n");
  }

  dirp->size = 0;

  printf("allocate succeeds\n");

  free(a);

  free(b);

  free(dirp);

  printf("free done\n");


  while(1);
*/




/*
  
  char a[10];
  if(read(fd, a, 10) < 0)
  {
    write(1, "b", 1);
    while(1);
  }

  if(write(1, a, 10) < 0)
  {

  }
  
  while(1);
*/
/*
  char *file = "hello";
  execve(file, NULL, NULL);
  } 

  	
  int *addr = (int *)0xF000000UL;

  //testing sucess cond
  brk(addr); 

  //testing failure cond
  addr = (int *) -1;
  brk(addr);

  //while(1);
  
  char *file1 = "hello";
  char *args1[] = {"hello", "arg1", "arg2", NULL};
  char *envs1[] = {"key=value", NULL};
  
  execve(file1, args1, envs1); //this passes

  
  char *file2 = "/bin/ls";

  int ret = execve(file2, NULL, NULL); //this fails 
  if(ret < 0)
	  while(1);

*/
  //while(1)
  //char *args[] = {"/bin/ls", "-lR", NULL};
  //execve(args[0], args, NULL);

  //this should definitely give segfault
  //*addr = 0x1234;
 
  
   
  char *path_arr[] = {"usr/", "/", NULL};

  int n = 0;
  while(path_arr[n])
{
  char *path = path_arr[n];

  struct dstream *dir;
  printf("start openddir for \"%s\"\n", path); 
  dir = opendir(path); 
  if(!dir)
  {
	 printf("opendir failed\n");
  }
    
  printf("start readdir\n");
    
  struct linux_dirent *drent;
  while((drent = readdir(dir)) != NULL) 
  {
	printf("\nreading file: %s\n", drent->d_name);
  }
  printf("done readdir\n");
  
  printf("start closedir\n");
  closedir(dir);
  printf("done closedir\n");
  n++;
}


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
  

  while(1);

  return 0;

}
