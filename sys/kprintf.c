#include <sys/kprintf.h>

#define WIDTH 160
#define HEIGHT 25
#define SIZE ((WIDTH)*(HEIGHT))
#define BASE 0xb8000


int x = 0,y = 0;
char arr[SIZE];


void kprintf(const char *fmt, ...)
{

}


void kputchar(const char c)
{
    if(c == '\n'){
        x=0;
        y++;
    }else if(c == '\r'){
        x=0;
    }else if(c >= ' '){
        *(((char*)BASE)+x*2+y*WIDTH)=c;
        arr[x+y*WIDTH]=c;
        x++;
    }

    if(x >= WIDTH){
        x=0;
        y++;
    }

    //TODO implement scroll up
}

void kputs(const char* str){
    const char *s;
    for(s=str; *s; ++s)
        kputchar(*s);
}
