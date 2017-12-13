#include <stdlib.h>
#include <stdio.h>
#include <syscall.h>
#include <dirent.h>

int main(int argc, char *argv[], char *envp[])
{
   __asm__ volatile ("movq $50, %rax");
   __asm__ volatile ("movq $60, %rbx");

  __asm__ volatile ("int $0x80");

  test(7000);
  //yield(); // lol that'd be too easy...
  
 // printf("just wanna test opendir\n");
  //opendir("/rootfs");
  while(1);

  return 0;

}
