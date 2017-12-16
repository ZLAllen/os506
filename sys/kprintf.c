#include <sys/system.h>
#include <sys/stdarg.h>
#include <sys/kprintf.h>
#include <sys/files.h>

#define WIDTH 80
#define HEIGHT 24
#define SIZE ((WIDTH)*(HEIGHT))
#define BLACK 0x0700

#define CLOCK_OFFSET 15

#define TERMBUF_SIZE 256

typedef struct term_buf{
  int start;
  int end;  //double pointer
  char echo; //echo enable
  char full; // full indicator
  volatile char count; //number of input delims;
  int backspace; // number of backspace can have, probably won't need
  char buf[TERMBUF_SIZE]; //holds all the read data
}terminal;

// define one instance of the terminal used by all process(probably only shell
// for input
terminal term = {
  .start = 0,
  .end = 0,
  .echo = 1,
  .full = 0,
  .count = 0,
  .backspace = 0,
  .buf = {0}
}; // dont know why I have to use designated assignment, stupid compiler 

struct file_ops term_op = {
  .open = 0, // open doesn't make sense to me, just call it from terminal.c
  .close = term_close,
  .read = term_read,
  .write = term_write,
  .readdir = term_readdir  // this one just return error
};


// the file object that connects to terminal
struct file term_file = {
  .fop = &term_op, //pointer to struct of file functions
  .count = 1, // this file will be hold by at least 1 process, never die until system shutdown
  .offset = 0, // not useful in terminal
  .data = &term, // this points to the terminal object
  .size = 0 // not useful in terminal
};


int x = 0,y = 0;
short arr[SIZE];

static char Rep[] = "0123456789ABCDEF";

static void kputTime(int integer, short* loc);
static void convert(unsigned long i, int base);
static void updatecsr();

//terminal static function
static int term_pop(); //pop the first character available, return 0 at delim
static void term_push(const char a); //push a character into the term_buf, called by term_putchar, which is used during keyboard interrupt
static void term_poplast();


struct file* term_open(const char* path, int flag)
{
  // increment terminal file object reference count then return pointer to the
  // object
  term_file.count++;
  return &term_file;
}  

int term_close(struct file* file)
{
  // in close syscall must check fd != 0 first
  term_file.count--;
  return 0; 
}

/* func: term_pop
 *
 * @return the first character available to pop, -1 if none
 */
static int term_pop()
{
  if(term.start == term.end && !term.full)
  {
    return -1;  // empty, nothing to pop 
  }

  char c = term.buf[term.start];
  
  //move cursor to the next position, wrap around if needed
  term.start = (term.start + 1) % TERMBUF_SIZE;

  // clear any full flag
  term.full = 0;

  if(c == EOT || c == '\n')
  {
    // decrement 1 delim count upon end of pop sequence
    term.count--;
  }

  return c;
}


/* func: term_read
 * fp: file object that holds the terminal buf
 * buf: buf to receive data
 * size: size of the data to read
 * offset: offset into the file, not useful here
 *
 * @return: the actual numebr of bytes read
 */
ssize_t term_read(struct file* fp, char* buf, size_t size, off_t* offset)
{
  if(!fp || !buf) // if file object and buf not valid
  {
    return -1; // setting error code later
  }

  if(size == 0)
    return 0;

  int count = 0;
  while(count <= size)
  {
    int c = term_pop();
    if(c < 0)
    {
      break; // empty
    }
    buf[count++] = (char)c;

    if(c == EOT || c == '\n')
    {
      break; //stops here at delims
    }
  }
  return count;
}
/* func term_write
 * fp: pointer to file object not useful
 * buf: user buffer with data
 * count: number of bytes to output 
 * offset: not useful
 */ 
ssize_t term_write(struct file* fp, char* buf, size_t count, off_t * offset)
{
  if(!fp || !buf)
  {
    return -1; // same error as in read
  }

  //simply print to console, sorry didn't buffer it
  kputs(buf);

  return count;
}


int term_readdir(struct file* fp, void* buf, unsigned int count)
{
  return -1; // this is an illegal operation
} 

/*
 * keyboard interrupt call this function to place a character into term_buf
 *
 * a: the character to put
 * @return: 0 if succeed, -1 if failed
 *
 */
int term_putchar(char a)
{
  //since we have only one foregrond process, do not implement EOT
  if(a == '\b')
  {
    // now we implement backspace, first look at how much we can backspace
    if(term.backspace == 0)
    {
      return -1;
    }

    term_poplast();
    // check echo 
    if(term.echo)
    {
      //reflect this on display
      kputchar('\b');
    }
  }

  // reject inputs upon full
  if(term.full)
  {
    return -1;
  }

  // add to buffer
  
  if(a == '\t')
  {
    int count = 0;
    while(count++ < 4)
    {
      term_push(' ');
    }
    term.backspace += 4;
  }else{
    term.backspace++;
    term_push(a);
  }

  if(term.echo)
    kputchar(a);

  return 0;
}

/*
 *
 * remove the most recent character from the terminal buffer
 *
 */

static void term_poplast()
{
  // here we need to be careful, decrementing end can result in negative value,
  // which is undesire, but adding TERMBUF_SIZE-1 will wrap the value around
  term.end = (term.end + TERMBUF_SIZE-1 )% TERMBUF_SIZE; 
  term.full = 0; // full must not be true 
}





/*
 * push a character into the terminal buffer
 * move the end pointer to the next position,
 * update any full status
 *
 * a: character to push
 * @return: 0 if succeed, -1 otherwise
 */
static void term_push(const char a)
{

  term.buf[term.end] = a;

  term.end = (term.end + 1) % TERMBUF_SIZE;

  if(term.start == term.end)
  {
    term.full = 1;
  }

  if(a == '\n' || a == EOT)
  {
    term.count++;
    term.backspace = 0; //reset backspace upon receiving delim
  }
}

void kprintf(const char *fmt, ...)
{
    const char* ptr;
    unsigned int i;
    char* s;
    unsigned long p;



    va_list arg;
    va_start(arg, fmt);

    ptr = fmt;
    while(*ptr)
    {
        // check for formatter
        if(*ptr != '%') 
        {
            kputchar(*ptr);
        }
        else
        {
            switch(*(++ptr))
            {
                case 'c':
                    i = va_arg(arg, int);
                    kputchar((char)i);
                    break;
             
                case 'd':
                    i = va_arg(arg, int);
                    convert(i, 10);
                    break;

                case 's':
                    s = va_arg(arg, char*);
                    kputs(s);
                    break;

                case 'x':
                    p = va_arg(arg, unsigned long);
                    convert(p, 16);
                    break;

                case 'p':
                    p = va_arg(arg, unsigned long);
                    kputs("0x");
                    convert(p, 16);
                    break;
            }
        }
        ++ptr;
    }

    va_end(arg);

}

static void convert(unsigned long i, int base)
{
    char* ptr;
    static char buf[50];

    ptr = &buf[49];
    *ptr = '\0';

    do
    {
        *--ptr = Rep[i%base];
        i /= base;
    }while(i != 0);

    kputs(ptr);
}


void kputchar(const char c)
{
    if(c == '\n'){
        x=0;
        y++;
    }else if(c == '\r'){
        x=0;
    }else if(c >= ' '){
        *(((char*)BASE)+x*2+y*WIDTH*2) = c;
        arr[x+y*WIDTH] = BLACK|(short)c;
        x++;
    }else if(c == '\b'){ 
      // move the x cursor first  
      if(x == 0)
      {
        y--;
        x = WIDTH-1;
      }
      else
      {
        x--;
      }
      // clear the backspace
       *(((char*)BASE)+x*2+y*WIDTH*2) = ' ';
       arr[x+y*WIDTH] = BLACK|0x20;
    }else if(c == '\t'){
      // move cursor 4 blank space forward
      int cnt = 0;
      while(cnt < 4)
      {
        kputchar(' ');
      }
    }else{
      return;
    }  

    if(x >= WIDTH){
        x=0;
        y++;
    }


    if(y >= HEIGHT){
        //scroll up screen first
        memmove(arr+WIDTH,(void*)BASE, SIZE*2-WIDTH*2);
        memsetw((short*)BASE+SIZE-WIDTH, BLACK|0x0020,WIDTH); 

        //plit pdate local buffers
        memmove(arr+WIDTH, arr, SIZE*2-WIDTH*2);
        memsetw(arr+SIZE-WIDTH, BLACK|0x0020, WIDTH);

        y = HEIGHT-1;
        x = 0;
    }

    updatecsr();

}

void updatecsr(){
    unsigned short temp;

    temp = y*80 + x;

    outb(0x3D4, 14);
    outb(0x3D5, temp >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, temp);
}

void kputs(const char* str){
    const char *s;
    for(s=str; *s; ++s)
        kputchar(*s);
}

void update_time(uint32_t day, uint32_t sec){
    int minute, hour, second;
    short* loc = (short*)BASE + 80*25 - CLOCK_OFFSET;

    //should define a struct to host it sometime
    hour = sec/3600;
    minute = (sec%3600)/60;
    second = (sec%3600)%60;

    kputTime(hour, loc);
    *(loc+2) = BLACK|0x003A;
    kputTime(minute, loc+3);
    *(loc+5) = BLACK|0x003A;
    kputTime(second, loc+6);
}

static void kputTime(int integer, short* loc){
    char digit = '0';
    char digits[25];
    int c = 0;
    short* ptr;

    ptr = loc;

    if(integer<10){
        digit += integer;
        *ptr++ = BLACK|0x0030;
        *ptr = BLACK|digit;
        return;
    }
    else
    {
        while(integer){
            // get the digit one by one
            digits[c++]=integer % 10;
            integer /= 10;
        }
        while(c > 0){
            c--;
            digit = digits[c] + '0';
            *ptr++ = BLACK|digit;
        }
    }
}

void clr(){
    int i;
    unsigned short blank;

    blank = BLACK|0x20;

    for(i = 0; i < 24; i++){
        memsetw((short*)BASE+i*80, blank, 80); 
        memsetw(arr+i*80, blank, 80);
    }

    x=0;
    y=0;
    updatecsr();
}
