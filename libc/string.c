#include <sys/defs.h>

// follows standard C string signatures

// compare two strings
int strcmp(const char *str1, const char *str2) {

    char *current1 = (char *)str1;
    char *current2 = (char *)str2;

    while (*current1 && *current2 && *current1 == *current2) {
        current1++;
        current2++;
    }

    return *current2 - *current1;
}

// find next token in string
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

// length of null-terminated string
size_t strlen(const char *str) {

    char *current = (char *)str;
    int len = 0;

    while (*current++)
        len++;

    return len;
}

// copy from sourc to destination string
char* strcpy(char *target, const char *src)
{
	int i;
	for (i = 0; src[i] != '\0'; i++)
		target[i] = src[i];
	// terminating null byte
	target[i] = '\0';
	return target;
}

// copy n bytes from source to destination string
char* strncpy(char *target, const char *src, int n)
{
	// might be buggy if destination is not large enough
	int i;
	for (i = 0; i < n && src[i] != '\0'; i++)
		target[i] = src[i];
	// might not be null terminated if not in first n bytes
	for ( ; i < n; i++)
		target[i] = '\0';
	return target;

}

