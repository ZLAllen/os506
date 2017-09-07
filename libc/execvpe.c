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
        if(strcmp(environ[c], name) == 0) {
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

    int ret = -1;
    int fd, slash;
    char* path, *pbuffer, *token;
    char delim;
    char* ctoken, *citoken;

    // check if the file is NULL
    if (*file == '\0') {
        printf("bad file\n");
        return -1;
    }

    slash = 0;

    // don't search for the binary; if fail set the slash flag
    if (strchr(file, '/') != NULL){
        int fd = open(file, 0);
        if(fd > 0){
            ret = execve(file, argv, envp);
            return ret;
        }else
            // set the flag and sarch in the PATH
           slash = 1;
    }

    path = getenvval("PATH");
    // if path is NULL use a default value
    if (!path)
        path=DEFAULT_PATH;

    pbuffer = malloc(strlen(path) + strlen(file) + 1);
    strcpy(pbuffer, path);

    // get each path and search for the binary
    token = strtok(pbuffer, ":");
    delim = '/';
    while (token){
        token = strtok(NULL, ":");
        // concat token and the actual path
        citoken = NULL;
        if(slash != 1) {
            citoken = concat(token, &delim);
            ctoken = concat(citoken, file);
        }
        else
            ctoken = concat(token, file);

        fd = open(ctoken, 0);

        printf("executing:%s\n", ctoken);
        if (fd > 0) {
            printf("executing:%s\n", ctoken);
            ret =  execve(ctoken, argv, envp);
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
    printf("failed and return\n");
    return ret;
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

