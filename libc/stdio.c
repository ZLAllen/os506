#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>


static char Rep[] = "0123456789ABCDEF";

static void convert(unsigned long i, int base);

/*
void handleInteger(int integer);

int printf(const char *format, ...) {

    va_list arguments;
    // initialize arguments to store all values after format
    va_start(arguments, format);

    int num_char = 0;
    // just handle %s and %d
    while(*format) {
        if(*format == '%') {
            format++;

            if(*format == 's') {
                char* next_val_ptr = va_arg(arguments, char*);
                while(*next_val_ptr != '\0') {
                    write(1, next_val_ptr, 1);
                    next_val_ptr++;
                }
            }
            else if(*format == 'd') {
                    int next_val = va_arg(arguments, int);
                    handleInteger(next_val);
            }
        } else {
            write(1, format, 1);
        }
        num_char++;
        format++;
    }
    // clean up the list
    va_end(arguments);

    // return the num of characters printed
    return num_char;
}


void handleInteger(int integer) {
    if(integer==0){
        int digit = '0';
        write(1, &digit, 1);
        return;
    }
    else
    {
        if(integer<0){
            char sign = '-';
            write(1, &sign, 1);
            integer=-integer;
        }
        int digits[25];
        int c=0;
        while(integer){
            // get the digit one by one
            digits[c++]=integer % 10;
            integer /= 10;
        }
        while(c > 0){
            c--;
            int digit = digits[c] + '0';
            write(1,  &digit, 1);
        }
    }

}
*/

int printf(const char *fmt, ...)
{
    const char* ptr;
    unsigned int i;
    char* s;
    unsigned long p;
    int num_char = 0;



    va_list arg;
    va_start(arg, fmt);

    ptr = fmt;
    while(*ptr)
    {
        // check for formatter
        if(*ptr != '%') 
        {
            putchar(*ptr);
        }
        else
        {
            switch(*(++ptr))
            {
                case 'c':
                    i = va_arg(arg, int);
                    putchar((char)i);
                    break;
             
                case 'd':
                    i = va_arg(arg, int);
                    convert(i, 10);
                    break;

                case 's':
                    s = va_arg(arg, char*);
                    puts(s);
                    break;

                case 'x':
                    p = va_arg(arg, unsigned long);
                    convert(p, 16);
                    break;

                case 'p':
                    p = va_arg(arg, unsigned long);
                    puts("0x");
                    convert(p, 16);
                    break;
            }
        }
        ++ptr;
    }

    va_end(arg);

    return num_char;

}

static void convert(unsigned long i, int base)
{
    char* ptr;
    char buf[50];

    ptr = &buf[49];
    *ptr = '\0';

    do
    {
        *--ptr = Rep[i%base];
        i /= base;
    }while(i != 0);

    puts(ptr);
}


