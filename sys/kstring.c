#include <sys/kstring.h>


int kstrlen(const char *str)
{
    int length=0;
    while (*str++ != '\0')
        length++;
    return length;
}


// find next token in string
char *kstrtok(char *str, const char *delimiters) {

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
    } 
	else 
	{
        return 0;
    }
}
