#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <syscalls.h>

#define BUFSIZE 512

void grep(char *pattern, int fd) {
    // pretty inefficient solution for now
    
    char buf[BUFSIZE];

    int bufferLen;
    int patternLen = strlen(pattern);

    while ((bufferLen = read(fd, buf, BUFSIZE)) > 0) {
        
        // search buffer for pattern
        int i, j;

        // count how many matches per buffer
        int count = 0;

        // move one character at a time
        for (i = 0; i < bufferLen - patternLen; i++) {

            // check if string starting at character i is the pattern
            for (j = 0; j < patternLen; j++) {
                // check if the current character matches pattern
                if (buf[i + j] != pattern[j])
                    break;
            }

            // was a match found?
            if (j == patternLen) {
                count++;
            }
        }

        // print if match
        if (count > 0) {
            printf("%d matches found:\n", count);
            printf("%s\n", buf);
        }

    }
}

int main(int argc, char *argv[], char *envp[]) {

    int fd;

    // get pattern
    if (argc < 2) {
        // error for pattern required
        printf("Pattern required");
    } else if (argc < 3) { 
        // read from file if provided, else STDIN
        grep(argv[1], 0);
    } else {

        // read multiple files
        for (int i = 2; i < argc; i++) {
            fd = open(argv[i], O_RDONLY);
            if (fd < 0) {
                // error for invalid file
                printf("Invalid file");
                exit(1);
            }

            grep(argv[1], fd);
            close(fd);
        }
    }
    exit(0);
}
