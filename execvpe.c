#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

extern char **environ;

// default path when PATH is NULL
#define DEFAULT_PATH "/bin:/usr/bin"


int execve(const char *filename, char *const argv[], char *const envp[]);


char* getenvval(char* name){
    int c = 0;
    while(*environ[c] != 0) {
        if(strncmp(environ[c], name, strlen(name)) == 0) {
            // key=value; get the value
            return environ[c]+strlen(name)+1;
        }
        c++;
    }
    return NULL;
}



char* concat(const char *s1, const char *s2)
{
    // length +1 for the NULL-terminator
    char *result = malloc(strlen(s1)+strlen(s2)+1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

int execvpe(const char *file, char *const argv[], char *const envp[]){

    // check if the file is NULL
    if (*file == '\0') {
        return -1;
    }

    int slash = 0;

    // don't search for the binary; if fail set the slash flag
    if (strchr(file, '/') != NULL){
        int fd = open(file, 0);
        if(fd > 0)
            execve(file, argv, envp);
        else
            // set the flag and sarch in the PATH
           slash = 1;
    }

    char *path = getenvval("PATH");
    // if path is NULL use a default value
    if (!path)
        path=DEFAULT_PATH;

    char pbuffer[strlen(path) + strlen(file) + 1];
    strcpy(pbuffer, path);

    // get each path and search for the binary
    char* token = strtok(pbuffer, ":");
    char delim = '/';
    while (token){
        token = strtok(NULL, ":");
        // concat token and the actual path
        char *ctoken;
        char *citoken = NULL;
        if(slash != 1) {
            citoken = concat(token, &delim);
            ctoken = concat(citoken, file);
        }
        else
            ctoken = concat(token, file);

        int fd = open(ctoken, 0);
        if (fd > 0) {
            //printf("executing:%s\n", ctoken);
            execve(ctoken, argv, envp);
            if(!citoken)
                free(citoken);
            free(ctoken);
            break;
        }
        else {
            //printf("failed:%s\n", ctoken);
            continue;
        }
    }
    free(pbuffer);
}

/*
void main(){

    char *path = getenv ("PATH");
    char s[256];
    strcpy(s, path);
    char* token = strtok(s, ":");
    while (token) {
        token = strtok(NULL, ":");
    }

    // /bin/ls and /ls and ls work
    char *args[2];
    args[0] = "/bin/ls";
    args[1] = NULL;

    execvpe(args[0], args, NULL);
};
*/

