#include <stdio.h>
#include <syscall.h>

int putchar(int c)
{
  char a = (char)c;
  // write character to stdout
  write(1, &a, 1); 
  return c;
}
