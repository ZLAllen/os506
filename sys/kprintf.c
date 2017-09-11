#include <sys/kprintf.h>
#include <sys/kstring.h>

#define WIDTH 80
#define HEIGHT 25
#define SIZE ((WIDTH)*(HEIGHT))
#define BASE 0xb8000
#define BLACK 0x0700

int x = 0,y = 0;
short arr[SIZE];



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
        *(((char*)BASE)+x*2+y*WIDTH*2) = c;
        arr[x+y*WIDTH] = BLACK|(short)c;
        x++;
    }

    if(x >= WIDTH){
        x=0;
        y++;
    }

    //TODO implement scroll up

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

}

void kputs(const char* str){
    const char *s;
    for(s=str; *s; ++s)
        kputchar(*s);
}
