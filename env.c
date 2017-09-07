#include <stdio.h>
#include <string.h>

extern char **environ;
char *getenv(char *name);


int setenv(char *name, char *value, int overwrite){
    char *val = getenv(name);
    printf("old value:%s\n", val);
    if(overwrite == 1) {
        memset(val, 0, sizeof(val));
        strcpy(val, value);
    }
    printf("new value:%s\n", val);
    return 0;
}

char *extractFromEnviron(char *var){
    int c = 0;
    while(*environ[c] != 0) {
    if(strcmp(environ[c], var) == 0) {
    // key=value; get pointer to the value
    return environ[c]+strlen(var)+1;
    }
    c++;
    }
    return NULL;
}


char *getenv(char *name){

    // env handling
    char* user="USER";
    char* home="HOME";
    char* path="PATH";

    char *PATH = extractFromEnviron(path);
    char *USER = extractFromEnviron(user);
    char *HOME = extractFromEnviron(home);

    if(strcmp(name,"HOME") == 0){
        return HOME;
    }else if(strcmp(name,"PATH") == 0){
        return PATH;
    }else if (strcmp(name,"USER") == 0){
        return USER;
    }else{
        // how to handle PS1
    }
}

/*
int main(int argc, char *argv[], char*envp[])
{
    char *name= "PATH";

    char *PATH = getenv(name);

    printf("PATH:%s\n", PATH);

    char *value = "leenashekharsingh";
    setenv(name, value, 0);
    setenv(name, value, 1);
    return 0;
}*/

