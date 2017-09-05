#include "stdio.h"
#include "syscalls.h"

int putchar(int c)
{
  // write character to stdout
  write(1,(char*)(&c), 1); 
  return c;
}
