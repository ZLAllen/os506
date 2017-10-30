#include <sys/system.h>
#include <sys/stdarg.h>
#include <sys/kprintf.h>

#define WIDTH 80
#define HEIGHT 24
#define SIZE ((WIDTH)*(HEIGHT))
#define BASE (KERN + 0xb8000)
#define BLACK 0x0700

#define CLOCK_OFFSET 15

int x = 0,y = 0;
short arr[SIZE];

static char Rep[] = "0123456789ABCDEF";

static void kputTime(int integer, short* loc);
static void convert(unsigned long i, int base);
static void updatecsr();
        

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
                    i = va_arg(arg, unsigned int);
                    convert(i, 16);
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
