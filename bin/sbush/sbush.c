
#include <stdlib.h>


int main(int argc, char *argv[], char *envp[])
{
   __asm__ volatile ("movq $50, %rax");

  __asm__ volatile ("int $0x80");
  
  while(1);

  return 0;

}
