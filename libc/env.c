#include <stdio.h>
#include <string.h>

char **environ;
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

    char *pa, *us, *hs, *ps;
    // env handling
    char* user="USER";
    char* home="HOME";
    char* path="PATH";
    char* ps1 ="PS1";

    pa = extractFromEnviron(path);
    us = extractFromEnviron(user);
    hs = extractFromEnviron(home);
    ps = extractFromEnviron(ps1);

    if(strcmp(name,"HOME") == 0){
        return hs;
    }else if(strcmp(name,"PATH") == 0){
        return pa;
    }else if(strcmp(name,"USER") == 0){
        return us;
    }else if(strcmp(name,"PS1") == 0){
        return ps;
    }

    return 0;
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

