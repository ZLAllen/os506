#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>


// will use it to resolve the file
#define DEFAULT_PATH "/bin:/usr/bin"
extern char **environ;


int execve(const char *filename, char *const argv[], char *const envp[]);

// implement execvpe to resole path first

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}


int execvpe(const char *file, char *const argv[], char *const envp[]){

    // check if the file is NULL
    if (*file == '\0') {
        return -1;
    }

    // don't search for the binary
    if (strchr(file, '/') != NULL){
        int fd = open(file, 0);
        if(fd < 0)
            return -1;
        execve(file, argv, envp);
    }

    // TODO replace getenv with custom setenv
    char *path = getenv ("PATH");
    // if path is NULL use a default value
    if (!path)
        path=DEFAULT_PATH;

    // filename and NULL
    size_t file_len = strlen(file) + 1;
    size_t path_len = strlen (path) + 1;

    // max string size
    char pbuffer[path_len + file_len + 1];
    strcpy(pbuffer, path);

    // get each path and search for the binary
    char* token = strtok(pbuffer, ":");
    char delim = '/';

    while (token) {
        token = strtok(NULL, ":");
        // concat token and the actual path
        char *citoken= concat(token, &delim);
        char *ctoken = concat(citoken, file);
        int fd = open(ctoken, 0);
        if (fd > 0) {
        // pbuffer points to the binary
            execve(ctoken, argv, envp);
            //printf("executing:%s\n", ctoken);
            break;
        }
        else {
            //printf("failed:%s\n", ctoken);
            continue;
        }
    }

}

void main(){

    char *path = getenv ("PATH");
    char s[256];
    strcpy(s, path);
    char* token = strtok(s, ":");
    while (token) {
        token = strtok(NULL, ":");
    }

    char *args[2];

    // /bin/ls and ls work
    args[0] = "ls";
    args[1] = NULL;

    execvpe(args[0], args, NULL);

};

