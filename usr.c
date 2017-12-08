int main()
{
  __asm__ volatile("hlt"); 
  // __asm__ volatile ("movq $50, %rax");

    //__asm__ volatile ("int $0x80");
  
    return 0;

}
