#include <sys/system.h>




/******************* String ******************/

static char Rep[] = "0123456789ABCDEF";

// a very dumbed down version of sprintf
// returns pointer to null terminated string version of int
// buf must be 50 or greater
char *int_to_str(char *buf, int i, int base) {

    char* ptr;

    ptr = &buf[49];
    *ptr = '\0';

    do {
        *--ptr = Rep[i%base];
        i /= base;
    } while(i != 0);
    return ptr;
}

// concatenate two strings
char *strcat(char *dest, const char *src) {
    // hold original dest to return
    char *cursor = dest;

    // move cursor to the end of dest
	while (*cursor)
		cursor++;

    // add each char of src to cursor
	while (*src) {
        *cursor = *src;
        cursor++;
        src++;
    }
	return dest;

}
char *strtok(char *str, const char *delimiters) {

    static char *position;

    // if str is NULL, use saved position
    if (str)
        position = str;
    
    int index = 0;
    char *currentDelimiter = (char *)delimiters;

    while (position[index]) {
        while (*currentDelimiter) {
            if (position[index] == *currentDelimiter) {
                position[index] = '\0';

                // save previous position before updating
                char *temp = position;

                // move position to index after delimiter
                position = position + index + 1;

                return temp;
            }
            currentDelimiter++;
        }
        currentDelimiter = (char *)delimiters;
        index++;
    }

    if (position[0]) {
        // reached end of string, move position and return start of final token
        char *temp = position;
        position += index;
        return temp;
    } else {
        return 0;
    }
}




/******************* MEMORY functions ************************/
void* memmove(void* src, void* dst, uint n){
    char*a = dst, *b = src;
    //handle overlap
    if(src>dst && src+n < dst){
        a = a+n;
        b = b+n;
        while(n-- > 0)
            *--a = *--b;
    }else{
        while(n-- > 0)
            *a++ = *b++;
    }

    return dst;
}

void* memsetw(const void* addr, int pattern, uint count){
    void* ret;
    __asm__ volatile
        ("cld; rep stosw"
            :"=D"(ret)
            :"D"(addr), "a"(pattern), "c"(count)
            :"cc", "memory"
         );

    return ret;
}

void* memset(const void* addr, int pattern, uint count){
    void* ret;
    __asm("cld; rep stosb"
            :"=D"(ret)
            :"D"(addr), "a"(pattern), "c"(count)
            :"cc", "memory"
         );

    return ret;
}


int memcmp(const void *str1, const void *str2, uint count)
{
	unsigned char c1, c2;
    	for ( ; count-- ; str1++, str2++) 
	{
		c1 = * (unsigned char *) str1;
		c2 = * (unsigned char *) str2;
		if ( c1 != c2) 
		{
	    		return (c1-c2);
		}
    	}
    return 0;	
}


void *memcpy(const void *src, void *dst, uint count)
{
	char *d = dst;
	const char *s = src;

	while (count--)
		*d++ = *s++;
	return dst;
}


void *memchr(const void *str, int ch, uint count)
{
	const unsigned char *p = str;
	while (count-- != 0)
	{
		if ((unsigned char)ch == *p++)
		{
			return (void *)(p - 1);
		}
	}
	return NULL;
}

/******************* IO functions ************************/


//follow GAS format
void outb(uint16_t port,uint8_t value) {
    
    __asm__ volatile ("out %0, %1" : : "a"(value), "d"(port));
}

unsigned char inb(uint16_t port){
    unsigned char value;
    __asm__ volatile ("in %1, %0" :"=a"(value): "d"(port));
    return value;
}

void out32(uint32_t port, uint32_t value) 
{
    __asm__ volatile ("outl %0, %w1" : : "a"(value), "Nd"(port));
}

uint32_t in32(uint32_t port)
{
    uint32_t value;
    __asm__ volatile ("inl %w1, %0" :"=a"(value): "Nd"(port));
    return value;
}




/*********** Registers **************/


