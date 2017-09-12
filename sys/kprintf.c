#include <sys/system.h>
#include <sys/defs.h>

#define WIDTH 80
#define HEIGHT 25
#define SIZE ((WIDTH)*(HEIGHT))
#define BASE 0xb8000
#define BLACK 0x0700

int x = 0,y = 0;
short arr[SIZE];


/*
void kprintf(const char *fmt, ...)
{
    va_list arguments;
	va_start(arguments, fmt);

	char char_val = 0;
	int int_val = 0;
	unsigned int_hex = 0
	char *str_val = NULL;


	while(*fmt){
		if(*fmt == '%'){
			char *spcfr = *(fmt+1);
			switch(spcfr){
				case 'c':
					char_val = va_arg(arguments, int);
					// handle char
					kputchar(char_val);
					break;
			    case 'd':
					int_val = va_arg(arguments, int);
					// handle int
					break;
				case 'x':
					hex_val = va_arg(arguments, unsigned int);
					/ handle hex
					break;
				case 's':
					str_val = va_arg(arguments, char*);
					//handle string
					break;
				case 'p':
					// handle a pointer?
					break;
				deafult:
					// some error message
					break;
			}
		}
		else{
			// print the character
		}
		fmt ++;
	}
}

void print_int(int val){
}

void print_hex(unsigned int val){
}

void print_str(char *val){
}*/


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
