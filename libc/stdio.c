#include <unistd.h>
#include <stdarg.h>



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




